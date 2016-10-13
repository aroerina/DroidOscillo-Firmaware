/*
 * @brief Hitex EVA 1850/4350 board file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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

#include "board.h"
#include "string.h"

/* Include other sources files specific to this board */
#include "retarget.h"

/** @ingroup BOARD_HITEX_EVA_18504350
 * @{
 */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define LED0_PORT  0
#define LED0_PIN   8

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System configuration variables used by chip driver */
//const uint32_t ExtRateIn = 0;
//const uint32_t OscRateIn = 12000000;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize pin muxing for a UART */
void Board_UART_Init(LPC_USART_T *pUART)
{
	if (pUART == LPC_USART0) {
	}
	else if (pUART == LPC_UART1) {
	}
	else if (pUART == LPC_USART2) {
		/* P2.10 : UART2_TXD J3 -> Pin 8, P2.11 : UART2_RXD ->  J3 Pin-7*/
		Chip_SCU_PinMuxSet(0x2, 10, (SCU_MODE_PULLDOWN | SCU_MODE_FUNC2));
		Chip_SCU_PinMuxSet(0x2, 11, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2));
	}
	else if (pUART == LPC_USART3) {
	}
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
	Board_UART_Init(DEBUG_UART);

	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaud(DEBUG_UART, 115200);
	Chip_UART_ConfigData(DEBUG_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);

	/* Enable UART Transmit */
	Chip_UART_TXEnable(DEBUG_UART);
#endif
}

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	/* Wait for space in FIFO */
	while ((Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	if (Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_RDR) {
		return (int) Chip_UART_ReadByte(DEBUG_UART);
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

/* Initializes board LED(s) */
static void Board_LED_Init()
{
	/* Set ports as outputs with initial states off */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, LED0_PORT, LED0_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, LED0_PORT, LED0_PIN, false);
}

/* Sets the state of a board LED to on or off */
void Board_LED_Set(uint8_t LEDNumber, bool On)
{
	/* Must connect JP3 to see LED0 and JP4 to see LED1 */
	if (LEDNumber == 0) {
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, LED0_PORT, LED0_PIN, On);
	}
}

/* Returns the current state of a board LED */
bool Board_LED_Test(uint8_t LEDNumber)
{
	bool On = false;

	if (LEDNumber == 0)
		On = (bool) Chip_GPIO_GetPinState(LPC_GPIO_PORT, LED0_PORT, LED0_PIN);

	return On;
}

void Board_LED_Toggle(uint8_t LEDNumber)
{
	Board_LED_Set(LEDNumber, !Board_LED_Test(LEDNumber));
}


/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Initializes GPIO */
	Chip_GPIO_Init(LPC_GPIO_PORT);

	/* Initialize LEDs */
	Board_LED_Init();
}

/* Sets up board specific ADC interface */
void Board_ADC_Init(void)
{
	/* Analog function ADC1_2 selected on pin PF_9 */
	Chip_SCU_ADC_Channel_Config(1, 2);
}

/* Sets up board specific I2C interface */
void Board_I2C_Init(I2C_ID_T id)
{
	if (id == I2C1) {
		/* Configure pin function for I2C1 on PE.13 (I2C1_SDA) and PE.15 (I2C1_SCL) */
		Chip_SCU_PinMuxSet(0xE, 13, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC2));
		Chip_SCU_PinMuxSet(0xE, 15, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC2));
	}
	else {
		Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
	}
}

/* Initialize pin muxing for SSP interface */
void Board_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP0) {
		/* Not available on the board */
		while (1) {}
	}
	else if (pSSP == LPC_SSP1) {
		/* Set up clock and muxing for SSP1 interface */
		/* SSEL: P1.20: J3 PIN-6 [Serial Expansion Interface] */
		Chip_SCU_PinMuxSet(0x1, 20, (SCU_PINIO_FAST | SCU_MODE_FUNC1));
		/* MISO: P1.3: J3 PIN-5 [Serial Expansion Interface] */
		Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5));
		/* MOSI: P1.4: J3 PIN-4 [Serial Expansion Interface] */
		Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5));
		/* SCLK: PF.4: J3 PIN-3 [Serial Expansion Interface] */
		Chip_SCU_PinMuxSet(0xF, 4, (SCU_PINIO_FAST | SCU_MODE_FUNC0));
	}
}

/* Initialize DAC interface for the board */
void Board_DAC_Init(LPC_DAC_T *pDAC)
{
	Chip_SCU_DAC_Analog_Config();
}

void Board_Buttons_Init(void)	// FIXME not functional ATM
{
}

uint32_t Buttons_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;
	return ret;
}

/* Initialize joystick interface on board */
void Board_Joystick_Init(void)
{}

/* Returns joystick states on board */
uint8_t Joystick_GetStatus(void)
{
	return NO_BUTTON_PRESSED;
}

/**
 * @}
 */
