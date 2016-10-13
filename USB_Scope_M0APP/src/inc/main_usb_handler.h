/*
 * main_usb_handler.h
 *
 *  Created on: 2016/03/16
 *      Author: AROE
 */

#ifndef INC_MAIN_USB_HANDLER_H_
#define INC_MAIN_USB_HANDLER_H_
#include <error.h>
#include "usbd_rom_api.h"

ErrorCode_t main_usb_handler(USBD_HANDLE_T hUsb, void *data, uint32_t event);

#endif /* INC_MAIN_USB_HANDLER_H_ */
