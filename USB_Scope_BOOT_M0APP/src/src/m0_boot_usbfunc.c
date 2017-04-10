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
#include "usbd_rom_api.h"
#include "boot_mc_shared_mem.h"
#include "main_usb_handler.h"
#include "m0_boot_usbfunc.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

extern const uint8_t HID_ReportDescriptor[];
extern const uint16_t HID_ReportDescSize;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

#ifdef RELEASE
/* HID Interrupt endpoint event handler. */
static ErrorCode_t HID_Ep_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return LPC_OK;
}
#endif

//外部プログラム読み込み用エンドポイント
// プロトコル
// 1 packet = 512byte
// 最初のパケット
// packet1   |書き込みアドレス 4byte(Little)|プログラムサイズ 4byte|プログラム本体|
// M4→M0→M0SUBの順でプログラム受信
#ifdef RELEASE
ErrorCode_t HID_EP2_Out_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{

	static uint8_t ep2out_receive_buffer[HID_EP2_OUT_PACKET_SIZE];
	static uint8_t received_packet_count = 0;
	static uint32_t write_addr=0,program_size=0,bytes_of_copyed=0;
	static uint8_t received_program_count=0,num_require_packet;

	switch (event) {

	case USB_EVT_OUT:
		break;

	case USB_EVT_OUT_NAK:	// Ready for Packet Receive
		USBD_API->hw->ReadReqEP(hUsb, HID_EP2_OUT, ep2out_receive_buffer, HID_EP2_OUT_PACKET_SIZE);

		break;

	case USB_EVT_OUT:
		USBD_API->hw->ReadEP(hUsb, HID_EP2_OUT, ep2out_receive_buffer);
		received_packet_count++;

		uint32_t source_addr,copy_size;

		if(received_program_count<3){
			if(received_packet_count == 1){	// 最初

				memcpy(&write_addr,&ep2out_receive_buffer[0],4);	// 書き込み開始アドレス
				memcpy(&program_size,&ep2out_receive_buffer[4],4);	// ファイルサイズ
				memcpy(&MC_EXCODE_INFO->ExfuncAddr[received_program_count],&ep2out_receive_buffer[8],4);

				MC_EXCODE_INFO->ExCodeStartAddr[received_program_count] = write_addr;
				MC_EXCODE_INFO->ExfuncAddr[received_program_count] |= 0x1;	// Use Thumb instruction flag set

				// 必要なパケット量を計算
				num_require_packet = ((program_size + 12 + HID_EP2_OUT_PACKET_SIZE-1)) / HID_EP2_OUT_PACKET_SIZE;	// 切り上げして さらに+ 1する

				source_addr = ((uint32_t)ep2out_receive_buffer) + 12;
				if(num_require_packet == 1){	// 1パケットで終わり
					copy_size = program_size;
				} else {
					copy_size = HID_EP2_OUT_PACKET_SIZE-12;
				}

			} else if(received_packet_count < num_require_packet){	// 真ん中
				source_addr = (uint32_t)ep2out_receive_buffer;
				copy_size = HID_EP2_OUT_PACKET_SIZE;

			}else if(received_packet_count >= num_require_packet){	//終わり
				source_addr = (uint32_t)ep2out_receive_buffer;
				copy_size = program_size - bytes_of_copyed;

			}

			// プログラムコピー
			memcpy( (void*)write_addr , (void*)source_addr , copy_size);
			write_addr += copy_size;
			bytes_of_copyed += copy_size;

			if(received_packet_count >= num_require_packet){	//終わり
				MC_EXCODE_INFO->ExCodeLoaded[received_program_count] = TRUE;
				received_program_count++;
				received_packet_count = 0;
				bytes_of_copyed = 0;

//				__SEV();

//				if(received_program_count==2){	// M0APPのプログラムを受信したら
//					USB_CORE_CTRL_T *pUsbCtrl = (USB_CORE_CTRL_T *) hUsb;
//					pUsbCtrl->ep_event_hdlr[2] = (USB_EP_HANDLER_T)(MC_EXCODE_INFO->ExfuncAddr[EXFUNC_M0]);	// ep1ハンドラーを書き換え
//					pUsbCtrl->ep_event_hdlr[3] = (USB_EP_HANDLER_T)(MC_EXCODE_INFO->ExfuncAddr[EXFUNC_M0]);
//				}
			}
		}

		break;
	}

	return LPC_OK;
}
#else

// debug用の偽物
ErrorCode_t HID_EP2_Out_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event){
	static uint8_t ep2out_receive_buffer[HID_EP2_OUT_PACKET_SIZE];

	switch (event) {
	case USB_EVT_OUT_NAK:	// Ready for Packet Receive
		USBD_API->hw->ReadReqEP(hUsb, HID_EP2_OUT, ep2out_receive_buffer, HID_EP2_OUT_PACKET_SIZE);
	case USB_EVT_OUT:
		USBD_API->hw->ReadEP(hUsb, HID_EP2_OUT, ep2out_receive_buffer);
		break;
	}

	return LPC_OK;
}

#endif

/*  HID get report callback function. */
static ErrorCode_t HID_GetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t *plength)
{
	return LPC_OK;
}

/* HID set report callback function. */
static ErrorCode_t HID_SetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t length)
{
	return LPC_OK;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* HID init routine */
ErrorCode_t usb_hid_init(USBD_HANDLE_T hUsb,
						 USB_INTERFACE_DESCRIPTOR *pIntfDesc,
						 uint32_t *mem_base,
						 uint32_t *mem_size)
{
	USBD_HID_INIT_PARAM_T hid_param;
	USB_HID_REPORT_T reports_data[1];
	ErrorCode_t ret = LPC_OK;

	memset((void *) &hid_param, 0, sizeof(USBD_HID_INIT_PARAM_T));
	/* HID paramas */
	hid_param.max_reports = 1;
	/* Init reports_data */
	reports_data[0].len = HID_ReportDescSize;
	reports_data[0].idle_time = 0;
	reports_data[0].desc = (uint8_t *) &HID_ReportDescriptor[0];

	if ((pIntfDesc == 0) || (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_HUMAN_INTERFACE)) {
		return ERR_FAILED;
	}

	hid_param.mem_base = *mem_base;
	hid_param.mem_size = *mem_size;
	hid_param.intf_desc = (uint8_t *) pIntfDesc;
	/* user defined functions */
	hid_param.HID_GetReport = HID_GetReport;
	hid_param.HID_SetReport = HID_SetReport;

#ifdef DEBUG
	hid_param.HID_EpIn_Hdlr  = main_usb_handler;
	hid_param.HID_EpOut_Hdlr = main_usb_handler;
#else
	hid_param.HID_EpIn_Hdlr  = HID_Ep_Hdlr;
	hid_param.HID_EpOut_Hdlr = HID_Ep_Hdlr;
#endif
	hid_param.report_data  = reports_data;

	ret = USBD_API->hid->init(hUsb, &hid_param);
	/* allocate USB accessable memory space for report data */
	//loopback_report =  (uint8_t *) hid_param.mem_base;
	hid_param.mem_base += 4;
	hid_param.mem_size += 4;
	/* update memory variables */
	*mem_base = hid_param.mem_base;
	*mem_size = hid_param.mem_size;

	return ret;
}

