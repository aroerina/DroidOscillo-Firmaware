/*
 * @brief This file contains USB Generic HID example using USB ROM Drivers.
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


// LPC4370 のM0にはSysTickなし

#include "board.h"
#include <string.h>
#include "app_usbd_cfg.h"
#include "m0_boot_usbfunc.h"
#include "mc_shared_mem.h"
#include "main_usb_handler.h"
#include "boot_mc_shared_mem.h"


#define GPIO_PORT_LED		0
#define GPIO_PIN_LED		15

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static USBD_HANDLE_T g_hUsb;

/* Endpoint 0 patch that prevents nested NAK event processing */
static uint32_t g_ep0RxBusy = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
static USB_EP_HANDLER_T g_Ep0BaseHdlr;	/* variable to store the pointer to base EP0 handler */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
const USBD_API_T *g_pUsbApi;
bool ConnectionEstablished;


/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* EP0_patch part of WORKAROUND for artf45032. */
ErrorCode_t EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{

//	if(ConnectionEstablished == false ){
//		ConnectionEstablished = true;			// USB接続確立
//		NVIC_DisableIRQ(TIMER0_IRQn);			// 割り込み停止
//		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED);		// LED
//		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED,false); //LED OFF
//	}

	switch (event) {
	case USB_EVT_OUT_NAK:
		if (g_ep0RxBusy) {
			/* we already queued the buffer so ignore this NAK event. */
			return LPC_OK;
		}
		else {
			/* Mark EP0_RX buffer as busy and allow base handler to queue the buffer. */
			g_ep0RxBusy = 1;
		}
		break;

	case USB_EVT_SETUP:	/* reset the flag when new setup sequence starts */
	case USB_EVT_OUT:
		/* we received the packet so clear the flag. */
		g_ep0RxBusy = 0;
		break;
	}
	return g_Ep0BaseHdlr(hUsb, data, event);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from USB0
 * @return	Nothing
 */
void USB_IRQHandler(void)
{
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;
	USBD_API->hw->ISR(g_hUsb);
}

/**
 * @brief	Find the address of interface descriptor for given class type.
 * @return	If found returns the address of requested interface else returns NULL.
 */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

//ErrorCode_t USBErrorHandler (USBD_HANDLE_T hUsb, uint32_t param1){
//	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED);		// LED
//	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED,false); //LED OFF
//
//	return LPC_OK;
//}

////接続再試行用
//void TIMER0_IRQHandler(void)
//{
//	static int x = 0;
//	Chip_TIMER_ClearMatch(LPC_TIMER0,0);	// Clear Interrupt Flag
//	if (x++ > 50) {		// 10ms*50 = 500ms　ごとに  *500ms以下だとうまくいかない
//		if(ConnectionEstablished == false){	// 接続を再試行
//			USBD_API->hw->Connect(g_hUsb, 0);	// 切断
//			//USBD_API->hw->Reset(g_hUsb);
//			USBD_API->hw->Connect(g_hUsb, 1);	// 接続
//		}
//
//		x = 0;
//	}
//}

//blinky
//void TIMER0_IRQHandler(void)
//{
//	Chip_TIMER_ClearMatch(LPC_TIMER0,0);	// Clear Interrupt Flag
//	static bool ledstate = false;
//	static int x = 0;
//	x++;
//	if (x > 50) {
//		ledstate = !ledstate;
//		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED,ledstate); 	// LED OFF
//		x = 0;
//	}
//}

/**
 * @brief	main routine for USB device example
 * @return	Function should not exit.
 */
volatile timescalse_div_val_T timescale_div_val[NUM_TIMESCALE];
volatile uint16_t sample_length[5];

// デバッグメモ　M0デバッグはデバッグの設定でLoad image を false にすること
#define TICKRATE_HZ1 (1000)

volatile uint8_t wait_debugger = 0;
int main(void)
{
	/* Initialize board and chip */
	SystemCoreClockUpdate();

	// LPC4370のM0にはSysTickタイマーなし
	// 接続再試行用タイマーセットアップa
//	Chip_TIMER_Init(LPC_TIMER0);
//	Chip_TIMER_SetMatch(LPC_TIMER0,0,SystemCoreClock/100);	// 一秒に100回
//	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0,0);
//	Chip_TIMER_MatchEnableInt(LPC_TIMER0,0);
//	Chip_TIMER_ClearMatch(LPC_TIMER0,0);	// Clear Interrupt Flag
//	NVIC_SetPriority (TIMER0_IRQn, 80);
//	NVIC_EnableIRQ(TIMER0_IRQn);
//	Chip_TIMER_Enable(LPC_TIMER0);

#ifdef RELEASE
	while(wait_debugger == 1);
#endif

	memset((void*)USB_STACK_MEM_BASE,0,USB_STACK_MEM_SIZE);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED);		// LED GPIO DIR SET OUT
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED,true); 	// LED ON

	//ConnectionEstablished = false;

	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T *pCtrl;

	/* enable clocks and pinmux */
	USB_init_pin_clk();

	/* Init USB API structure */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;
	usb_param.max_num_ep = NUM_ENDPOINT;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
	desc.string_desc = (uint8_t *) USB_StringDescriptor;

#ifdef USE_USB0
	desc.high_speed_desc = USB_HsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = (uint8_t *) USB_DeviceQualifier;
#else
	/* Note, to pass USBCV test full-speed only devices should have both
	 * descriptor arrays point to same location and device_qualifier set
	 * to 0.
	 */
	desc.high_speed_desc = USB_FsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = 0;
#endif

	/* USB Initialization */

	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);



	if (ret == LPC_OK) {

		/*	WORKAROUND for artf45032 ROM driver BUG:
		    Due to a race condition there is the chance that a second NAK event will
		    occur before the default endpoint0 handler has completed its preparation
		    of the DMA engine for the first NAK event. This can cause certain fields
		    in the DMA descriptors to be in an invalid state when the USB controller
		    reads them, thereby causing a hang.
		 */
		pCtrl = (USB_CORE_CTRL_T *) g_hUsb;	/* convert the handle to control structure */
		g_Ep0BaseHdlr = pCtrl->ep_event_hdlr[0];/* retrieve the default EP0_OUT handler */
		pCtrl->ep_event_hdlr[0] = EP0_patch;/* set our patch routine as EP0_OUT handler */

		ret = usb_hid_init(g_hUsb,
						   (USB_INTERFACE_DESCRIPTOR *) &USB_FsConfigDescriptor[sizeof(USB_CONFIGURATION_DESCRIPTOR)],
						   &usb_param.mem_base,
						   &usb_param.mem_size);
		if (ret == LPC_OK) {
			//Register extra code receive Endpoint
			ret = USBD_API->core->RegisterEpHandler(g_hUsb, HID_EP2_OUT*2, HID_EP2_Out_Hdlr, (void*)pCtrl);
			if(ret != LPC_OK){
				while(1);
			}
			USBD_API->hw->EnableEP(g_hUsb,HID_EP2_OUT);
			USBD_API->hw->EnableEvent(g_hUsb,HID_EP2_OUT,USB_EVT_OUT_NAK,1);

			/*  enable USB interrupts */
			NVIC_EnableIRQ(LPC_USB_IRQ);
			NVIC_SetPriority(LPC_USB_IRQ,128);

//			USBD_API->hw->EnableEvent(g_hUsb,0,USB_EVT_DEV_ERROR,1);
//			pCtrl->USB_Error_Event = USBErrorHandler;
			/* now connect */
			USBD_API->hw->Connect(g_hUsb, 1);	// Hostから見えるようになる
		}
	}

#ifdef RELEASE
	    while(MC_EXCODE_INFO->ExCodeLoaded[EXFUNC_M0] == FALSE){};	// M0コードが読み込まれるまで待機

	    ((void(*)(USBD_HANDLE_T))MC_EXCODE_INFO->ExfuncAddr[EXFUNC_M0])(g_hUsb);	// execute m0app_init();
#else
	    m0app_init(g_hUsb);
#endif

	while (1) {
		/* Sleep until next IRQ happens */
		__WFI();
	}
}
