/*
 * @brief This file contains USB HID Generic example using USB ROM Drivers.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "chip.h"
#include <stdint.h>
#include <string.h>
#include "app_usbd_cfg.h"
#include "usbd_rom_api.h"
#include "boot_mc_shared_mem.h"
#include "main_usb_handler.h"
#include "m0app_init.h"
#include "mc_shared_mem.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

extern const uint8_t HID_ReportDescriptor[];
extern const uint16_t HID_ReportDescSize;

USBD_HANDLE_T g_hUsb;
const USBD_API_T *g_pUsbApi;

/*****************************************************************************
 * Private functions
 ****************************************************************************/



//#ifdef DEBUG
//inline uint32_t calc_trigger_match_value(uint32_t f_adc,uint32_t f_timer,int16_t trigger_pos,uint32_t sample_length){
//	int16_t requisite_samples = (sample_length/2) - trigger_pos;	//画面右端までのサンプル数
//	return (uint32_t)((float)requisite_samples * ((float)f_timer / (float)f_adc));
//}

// 一番長いタイマー　水平トリガーを画面外左側の端に寄せた時　＝　1600サンプル
// Timerのプリスケール値が1でシステムクロックが204MHzの時の最長待ち時間　＝　21.053秒

#define WAITING_ADC_TRANSFER_SAMPLES 200	// ADC速度に応じて wait
#define WAITING_ADC_TRANSFER_TICKS   200	// 固定wait
void set_trigger_timer_regs(uint32_t f_adc,uint32_t f_timer,int16_t trigger_pos,uint32_t sample_length){

	if(MCV->Timescale >= TIMESCALE_500MS){			// クロックが遅い時はタイマープリスケール値セット
		LPC_TIMER1->PC = 0;			// Prescale counter reset
		LPC_TIMER1->PR = 1024 - 1;	// Prescale counter max value set = 1/1024
		f_timer = f_timer / 1024;
	} else {
		LPC_TIMER1->PC = 0;		// Prescale counter reset
		LPC_TIMER1->PR = 0;		// Prescale counter max value set = 1/1
	}

	int16_t requisite_samples = (sample_length/2) - trigger_pos;	//画面右端までのサンプル数
	if(requisite_samples < WAITING_ADC_TRANSFER_SAMPLES){
		requisite_samples = WAITING_ADC_TRANSFER_SAMPLES;
	} else {
		requisite_samples += WAITING_ADC_TRANSFER_SAMPLES;
	}

	volatile uint32_t ret = (uint32_t)((float)requisite_samples * ((float)f_timer / (float)f_adc));
	ret += WAITING_ADC_TRANSFER_TICKS;
	LPC_TIMER1->MR[0] =  ret;
}

void reset_sampler_main_loop();

#define MESSAGE_EEPROM_SEQ_READ 13
uint8_t UsbReceiveBuffer[HID_EP_OUT_PACKET_SIZE];
/* HID Interrupt endpoint event handler. */
//bool running = FALSE;
uint8_t i2c_buffer[I2C_BUFFER_SIZE];
uint8_t i2c_send_buffer;
ErrorCode_t main_usb_handler(USBD_HANDLE_T hUsb, void *usb_data, uint32_t event)
{

	switch (event) {

	case USB_EVT_IN_STALL:
	case USB_EVT_OUT_STALL:
		while(1);


	case USB_EVT_IN:		// host received data
		MCV->BufferIsPending = FALSE;
		break;

	case USB_EVT_OUT_NAK:	// Ready for Packet Receive
		USBD_API->hw->ReadReqEP(hUsb, HID_EP_OUT, UsbReceiveBuffer, HID_EP_OUT_PACKET_SIZE);
		break;		// return

	case USB_EVT_OUT:
			USBD_API->hw->ReadEP(hUsb,HID_EP_OUT, UsbReceiveBuffer);
			uint8_t message = UsbReceiveBuffer[0];
			uint8_t* data = &UsbReceiveBuffer[1];

				switch(message){
					case DATA_RECEIVED:
						MCV->BufferIsPending = FALSE;
						break;

					case UPDATE_FLAG_VERTICAL_TRIGGER:
						MCV->VTrigger = (data[1]<<8) | data[0];
						LPC_ADCHS->THR[0] = MCV->VTrigger;
						break;

					case UPDATE_FLAG_RUNNING_MODE:

						if (data[0] == RUNMODE_SINGLE){
							MCV->OneShotMode = TRUE;
							MCV->RunningMode = RUNMODE_NORMAL;
						} else if (data[0] == RUNMODE_SINGLE_FREE){
							MCV->OneShotMode = TRUE;
							MCV->RunningMode = RUNMODE_FREE;
						} else {
							MCV->RunningMode = data[0];
							MCV->OneShotMode = FALSE;

							// ロールモード設定
							if((MCV->RunningMode == RUNMODE_FREE) && (MCV->Timescale >= ROLEVIEW_SWITCH_TIMESCALE)){
								MCV->RoleModeOn = TRUE;
							} else {
								MCV->RoleModeOn = FALSE;
							}
						}

						// LED set
						// Running = ON ,Stop = OFF
						if(MCV->RunningMode != RUNMODE_STOP){
							set_LED_state(TRUE);
						} else {
							set_LED_state(FALSE);
						}


						reset_sampler_main_loop();
						break;

					case UPDATE_FLAG_HORIZON_TRIGGER:
						MCV->HTrigger = (data[1]<<8) | data[0];
						// timer match value set
						uint32_t divE_source_freq = (MCV->Timescale >= TIMESCALE_500MS) ? CRYSTAL_FREQ : USB_FREQ ;
						uint32_t adc_freq = divE_source_freq / (timescale_div_val[MCV->Timescale].divE_divider * timescale_div_val[MCV->Timescale].desc_div);
						set_trigger_timer_regs(adc_freq,SystemCoreClock,MCV->HTrigger,MCV->SampleLength);

						reset_sampler_main_loop();
						break;

					case UPDATE_FLAG_TIMESCALE:
						MCV->Timescale = *data;

						// サンプル長設定
						MCV->SampleLength = (MCV->Timescale >= TIMESCALE_1US)? MCV_BUFFER_SAMPLE_LENGTH : sample_length[MCV->Timescale];

						CHIP_CGU_CLKIN_T divE_clock_source;
						// ADCクロック源設定
						divE_clock_source = (MCV->Timescale >= TIMESCALE_500MS)? CLKIN_CRYSTAL : CLKIN_AUDIOPLL ;	//use audio pll

						// Div E divider set
						Chip_Clock_SetDivider(CLK_IDIV_E, divE_clock_source,timescale_div_val[MCV->Timescale].divE_divider);
						// set ADCHS Descriptor0 MATCH_VALUE
						LPC_ADCHS->DESCRIPTOR[0][0] = (1<<31)|(1<<24)|(0x1<<22)|((timescale_div_val[MCV->Timescale].desc_div-1)<<8)|(1<<6)|(0<<0);


						// ロールモード設定

						if((MCV->RunningMode == RUNMODE_FREE) && (MCV->Timescale >= ROLEVIEW_SWITCH_TIMESCALE)){
							MCV->RoleModeOn = 1;
						} else {
							MCV->RoleModeOn = 0;
						}

						uint8_t adchs_speed = (MCV->Timescale >= TIMESCALE_1US)? 0xE : 0 ;
						Chip_HSADC_SetSpeed(LPC_ADCHS,0,adchs_speed);

						reset_sampler_main_loop();
						break;

					case UPDATE_FLAG_BIAS_VOLTAGE:
						MCV->BiasVoltage = (data[1]<<8) | data[0];
						uint16_t inv = 4095 - MCV->BiasVoltage;		// 反転させる
						i2c_buffer[0] = (I2C_COMMAND_WRITE_VOLATILE_MEMORY << 5) | (0x3 << 3); // set vref is external buffered
						i2c_buffer[1] = (inv>>4) & 0xFF;				// MSB 左詰め
						i2c_buffer[2] = (inv & 0xF) << 4;			// LSB
						I2CTransferBlock(I2C_DAC_ADDR_7BIT, i2c_buffer, 3, NULL, 0);	// set DAC register

						break;


					case UPDATE_FLAG_VOLTAGE_SCALE:
						MCV->VoltageScale = *data;
						setup_input_switches();
						reset_sampler_main_loop();
						break;

					case UPDATE_FLAG_DC_CUT:
						MCV->DCCut = *data;
						setup_input_switches();
						break;

					case UPDATE_FLAG_TRIGGER_EDGE:
						MCV->TriggerEdge = *data;
						reset_sampler_main_loop();
						break;


					case MESSAGE_EEPROM_PAGE_WRITE:		// 指定のページに書き込み
						memcpy(i2c_buffer,data,(I2C_EEPROM_PAGE_SIZE+1));	// I2C書き込みバッファにコピー
						I2CTransferBlock(I2C_EEPROM_ADDR , i2c_buffer , (I2C_EEPROM_PAGE_SIZE+1) , NULL , 0); // パケット先頭のバイトでページ指定
						break;

					case MESSAGE_EEPROM_SEQ_READ:
						i2c_send_buffer = 0;	// word address
						const uint8_t read_bytes = *data;
						// interrupt i2cだと割り込みが起動しないからBlockじゃないとダメ
						I2CTransferBlock(I2C_EEPROM_ADDR, &i2c_send_buffer, 1, i2c_buffer, read_bytes);
						memcpy(MCV->Buffer,i2c_buffer,read_bytes);	//送信バッファにコピー
						USBD_API->hw->WriteEP(g_hUsb, HID_EP_IN, (uint8_t*)MCV->Buffer, read_bytes);		// USB送信
						break;


					default:
						break;
				}


				if(MCV->RunningMode == RUNMODE_FREE){
					MCV->RoleModeOn = (MCV->Timescale >= ROLEVIEW_SWITCH_TIMESCALE)? 1: 0;	// ロールモード設定

					if((message != UPDATE_FLAG_TIMESCALE) && (message != UPDATE_FLAG_RUNNING_MODE)){	// M4メインループをリセットしない
						MCV->UpdateFlags = 0;
						return LPC_OK;	// return
					}
				}

		break;

	}
	return LPC_OK;
}

void reset_sampler_main_loop (){
	//
	// M4メインループをリセットさせる処理
	//
	MCV->UpdateFlags = UsbReceiveBuffer[0];	//ステータスアップデートフラグセット
	MCV->BufferIsPending = FALSE;
	LPC_ADCHS->INTS[1].SET_STAT = HSADC_INT1_THCMP_UCROSS(0) | HSADC_INT1_THCMP_DCROSS(0);	// トリガー検出割り込みフラグセット
	LPC_GPDMA->CH[0].LLI = 0;
	LPC_TIMER1->TC = LPC_TIMER1->MR[0];		// Timerインタラプトフラグセット

	__SEV();	// M4 send event
}

