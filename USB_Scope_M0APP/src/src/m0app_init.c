#include "vector_table.h"
#include "m0app_init.h"
#include "mc_shared_mem.h"
#include "boot_mc_shared_mem.h"
#include "main_usb_handler.h"

#ifdef RELEASE
__attribute__ ((section(".isr_vector")))
uint32_t pseudo_vectortable[48];	//ベクターテーブル配置用空メモリ
#endif

// LPC LINK2 オシロスコープ基板を使うとき
//#define LPC_LINK2_OSCILLO_SCOPE

static I2CM_XFER_T  i2cmXferRec;
timescalse_div_val_T timescale_div_val[NUM_TIMESCALE];
uint16_t sample_length[5];
const USBD_API_T *g_pUsbApi;
volatile uint32_t time=0;	// 起動してからの時間

/*
 * GPIO接続
 * P2_5		GPIO5.5 : DC SWITCH
 * P2_6		GPIO5.6 : Low voltage opamp switch
 * P2_11	GPIO1.13 : MUX A
 * P2_12	GPIO1.12 : MUX S1
 * P2_13	GPIO1.11 : MUX S0
 * P1_20	GPIO0.15 : LED
 * P1_6		GPIO1.9 : EEPROM WP
 */

typedef struct {
	uint8_t port;
	uint8_t pin;
	uint8_t func;
	uint8_t gpio_port;
	uint8_t gpio_pin;
} GPIO_INITILIZER_T;

#define NUM_USE_GPIO_PINS		7
//		DC SWITCH PORT DIFINITION
//
#define PORT_DCSWITCH			2
#define PIN_DCSWITCH			5
#define FUNC_GPIO_DCSWITCH		FUNC4
#define GPIO_PORT_DCSWITCH 		5
#define GPIO_PIN_DCSWITCH		5

//		OPAMP SWITCH PORT DIFINITION
//
#define PORT_OPASWITCH			2
#define PIN_OPASWITCH			6
#define FUNC_GPIO_OPASWITCH		FUNC4
#define GPIO_PORT_OPASWITCH 	5
#define GPIO_PIN_OPASWITCH 		6

//		MUX S0 PORT DIFINITION
//
#define PORT_MUX_S0				2
#define PIN_MUX_S0				11
#define FUNC_GPIO_MUX_S0		FUNC0
#define GPIO_PORT_MUX_S0    	1
#define GPIO_PIN_MUX_S0 		11

//		MUX S1 PORT DIFINITION
//
#define PORT_MUX_S1				2
#define PIN_MUX_S1				12
#define FUNC_GPIO_MUX_S1		FUNC0
#define GPIO_PORT_MUX_S1   		1
#define GPIO_PIN_MUX_S1 		12

//		MUX A PORT DIFINITION
//
#define PORT_MUX_A			2
#define PIN_MUX_A			13
#define FUNC_GPIO_MUX_A		FUNC0
#define GPIO_PORT_MUX_A    	1
#define GPIO_PIN_MUX_A 		13

//		LED PORT DIFINITION
//
#define PORT_LED			1
#define PIN_LED				20
#define FUNC_GPIO_LED		FUNC0
#define GPIO_PORT_LED		0
#define GPIO_PIN_LED		15

//		EEPROM WP PORT DIFINITION
//
#define PORT_EEPROM_WP			1
#define PIN_EEPROM_WP			6
#define FUNC_GPIO_EEPROM_WP		FUNC0
#define GPIO_PORT_EEPROM_WP		1
#define GPIO_PIN_EEPROM_WP		9


// 一定時間ごとにバッファの状態をチェックしてサンプルを送信
void TIMER0_IRQHandler(void)
{
	Chip_TIMER_ClearMatch(LPC_TIMER0,0);	// Clear Interrupt Flag
	if(MCV->BufferIsPending == TRUE || MCV->RoleModeOn ){
		USBD_API->hw->WriteEP(g_hUsb, HID_EP_IN,(uint8_t*) MCV->Buffer, MCV_BUFFER_SIZE_BYTE);
	}

	time++;
}

// 外部DAC用I2Cハンドラ
void I2C0_IRQHandler(void)
{
	/* Call I2CM ISR function with the I2C device and transfer rec */
	Chip_I2CM_XferHandler(LPC_I2C0, &i2cmXferRec);
}

volatile uint8_t wait_debugger = 0;

#ifdef RELEASE
__attribute__ ((section(".after_vectors")))		// ベクターテーブル後に配置
#endif
void m0app_init(USBD_HANDLE_T hUsb){		// リンカのエントリポイントに書いておく

	Chip_Clock_SetBaseClock(CLK_BASE_APB1, CLKIN_MAINPLL, true, false);		//I2C APB1 Clock = MAIN CLOCK

	while(wait_debugger);	// wait debugger


	GPIO_INITILIZER_T gpio_init_array [NUM_USE_GPIO_PINS] = {		// port,pin,func,gpio_port,gpio_pin
		{2,5,4,5,5},		// dc switch
		{2,6,4,5,6},		// opa switch
		{2,11,0,1,11},		// mux s0
		{2,12,0,1,12},		// mux s1
		{2,13,0,1,13},		// mux a
		{1,20,0,0,15},		// LED
		{1,6,0,1,9}			// EEPROM WP
	};

	// Initialize GPIO PINS
	uint8_t i;
	for(i=0;i<NUM_USE_GPIO_PINS;i++){
		Chip_SCU_PinMuxSet(gpio_init_array[i].port,gpio_init_array[i].pin,gpio_init_array[i].func);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,gpio_init_array[i].gpio_port,gpio_init_array[i].gpio_pin);
	}




	/*		Time division change
			 val:td(sec):  freq : sample: 分周(480MHz/n)
				0:	 25n:	80M :	20
				1:	 50n:	80M :	40
				2:	100n:	80M	:	80	:6
				3:	250n:	80M	:	200	:6
				4:	500n:	80M	:	400	:6
				5:	  1u:	80M	:	800	:6
				6:	2.5u:	32M :	800	:15
				7:	  5u:	16M :	800	:30
				8:	 10u:	 8M	:	800	:60
				9:   25u:  3.2M	:		:150
			   10:   50u:  1.6M :		:300
			   11:  100u:  0.8M :		:600
			   12:  250u:  320k 		:1500
			   13:	500u:  160k 		:3000
			   14:	  1m:   80k			:6000
			   15:	2.5m:	32k			:15000
			   16:	  5m:	16k			:30000
			   17:   10m:	 8k			:60000
			   18:   25m:  3.2k	     	:150000
			   19:   50m:  1.6k			:300000
			   20:  100m:  0.8k			:600000
			   21:  250m:  320			:1500000
			   22:  500m:  160			:75000		// ここからDIVEクロック源をクリスタル(12MHz)に切り替え
			   23:    1 :   80			:150000
	 */
	sample_length[0] = 20;
	sample_length[1] = 40;
	sample_length[2] = 80;
	sample_length[3] = 200;
	sample_length[4] = 400;

	// divE_divider bit-width 8bit (max 255)
	// desc_div bit-width 14bit (max 8191)


	timescale_div_val[0].divE_divider = 6;
	timescale_div_val[0].desc_div = 1;
	timescale_div_val[1].divE_divider = 6;
	timescale_div_val[1].desc_div = 1;
	timescale_div_val[2].divE_divider = 6;
	timescale_div_val[2].desc_div = 1;
	timescale_div_val[3].divE_divider = 6;
	timescale_div_val[3].desc_div = 1;
	timescale_div_val[4].divE_divider = 6;
	timescale_div_val[4].desc_div = 1;
	timescale_div_val[5].divE_divider = 6;	// 1us
	timescale_div_val[5].desc_div = 1;
	timescale_div_val[6].divE_divider = 15;	// 2.5us
	timescale_div_val[6].desc_div = 1;
	timescale_div_val[7].divE_divider = 30; // 5us
	timescale_div_val[7].desc_div = 1;
	timescale_div_val[8].divE_divider = 60; // 10us
	timescale_div_val[8].desc_div = 1;
	timescale_div_val[9].divE_divider = 150;// 25us
	timescale_div_val[9].desc_div = 1;
	timescale_div_val[10].divE_divider = 50;// 50us
	timescale_div_val[10].desc_div = 6;
	timescale_div_val[11].divE_divider = 50;
	timescale_div_val[11].desc_div = 12;
	timescale_div_val[12].divE_divider = 50;
	timescale_div_val[12].desc_div = 30;
	timescale_div_val[13].divE_divider = 50;
	timescale_div_val[13].desc_div = 60;
	timescale_div_val[14].divE_divider = 50;
	timescale_div_val[14].desc_div = 120;
	timescale_div_val[15].divE_divider = 50;
	timescale_div_val[15].desc_div = 300;
	timescale_div_val[16].divE_divider = 50;
	timescale_div_val[16].desc_div = 600;
	timescale_div_val[17].divE_divider = 50;
	timescale_div_val[17].desc_div = 1200;
	timescale_div_val[18].divE_divider = 50;
	timescale_div_val[18].desc_div = 3000;
	timescale_div_val[19].divE_divider = 100;
	timescale_div_val[19].desc_div = 3000;
	timescale_div_val[20].divE_divider = 200;
	timescale_div_val[20].desc_div = 3000;
	timescale_div_val[21].divE_divider = 200;
	timescale_div_val[21].desc_div = 7500;
	timescale_div_val[22].divE_divider = 100;	// クロック源をクリスタル(12MHz)に切り替え
	timescale_div_val[22].desc_div = 750;
	timescale_div_val[23].divE_divider = 100;
	timescale_div_val[23].desc_div = 1500;


#ifdef RELEASE
	//ベクターテーブルコピー
	set_new_vtable(M0_BOOTCODE_ADDR,M0_EXCODE_ADDR);
	set_handler(M0_EXCODE_ADDR,TIMER0_IRQn,TIMER0_IRQHandler);	// register timer0 handler
	set_handler(M0_EXCODE_ADDR,I2C0_IRQn,I2C0_IRQHandler);	// register I2C handler
#endif

	//送信タイマー起動
	Chip_TIMER_Init(LPC_TIMER0);
	SystemCoreClock = 204000000;
	Chip_TIMER_SetMatch(LPC_TIMER0,0,SystemCoreClock/60);	// 一秒に60回
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0,0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0,0);
	NVIC_SetPriority (TIMER0_IRQn, 80);
	NVIC_EnableIRQ(TIMER0_IRQn);
	Chip_TIMER_Enable(LPC_TIMER0);

	// I2C setup
	Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
	Chip_I2C_Init(I2C0);
	Chip_I2C_SetClockRate(I2C0, SPEED_100KHZ);
	Chip_I2C_SetMasterEventHandler(I2C0, Chip_I2C_EventHandlerPolling);
//	NVIC_EnableIRQ(I2C0_IRQn);
//	NVIC_SetPriority (I2C0_IRQn, 90);	// NVIC優先度 0~255　数値が低い方が高い

	//while(time < 200);	// 待つ

	uint8_t command = (I2C_COMMAND_WRITE_VOLATILE_MEMORY << 5) | (0x3 << 3); // set vref is external buffered
	I2CTransferBlock(I2C_DAC_ADDR_7BIT, &command, 1, NULL, 0);

	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;		// USBD APIを使うのに必須
	USB_CORE_CTRL_T *pUsbCtrl = (USB_CORE_CTRL_T *) hUsb;
	g_hUsb = hUsb;
	pUsbCtrl->ep_event_hdlr[2] = main_usb_handler;	// ep1ハンドラーを書き換え
	pUsbCtrl->ep_event_hdlr[3] = main_usb_handler;



	while (1) {
		/* Sleep until next IRQ happens */
		__WFI();
	}
}

///* Function to wait for I2CM transfer completion */
//static void WaitForI2cXferComplete(I2CM_XFER_T *xferRecPtr)
//{
//	/* Test for still transferring data */
//	while (xferRecPtr->status == I2CM_STATUS_BUSY) {
//		/* Sleep until next interrupt */
//		__WFI();
//	}
//}


/* Function to setup and execute I2C transfer request */
void I2CTransfer(uint8_t devAddr,
								   uint8_t *txBuffPtr,
								   uint16_t txSize,
								   uint8_t *rxBuffPtr,
								   uint16_t rxSize)
{
	/* Setup I2C transfer record */
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.options = 0;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;
	Chip_I2CM_Xfer(LPC_I2C0, &i2cmXferRec);

	/* Wait for transfer completion */
	//WaitForI2cXferComplete(&i2cmXferRec);
}

void I2CTransferBlock(uint8_t devAddr,
								   uint8_t *txBuffPtr,
								   uint16_t txSize,
								   uint8_t *rxBuffPtr,
								   uint16_t rxSize)
{
	/* Setup I2C transfer record */
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.options = 0;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;
	Chip_I2CM_XferBlocking(LPC_I2C0, &i2cmXferRec);

	/* Wait for transfer completion */
	//WaitForI2cXferComplete(&i2cmXferRec);
}

void set_LED_state(bool state){	// TRUE = LED ON
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED,state);
}


void setup_input_switches(){

	// DC Switch
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_DCSWITCH,GPIO_PIN_DCSWITCH,!(MCV->DCCut));

	uint8_t vs = (MCV->VoltageScale > 7)? 7 : MCV->VoltageScale;
	// Low voltage opamp switch
	bool b_tmp = (vs & 0b100)? TRUE : FALSE;
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_OPASWITCH,GPIO_PIN_OPASWITCH,b_tmp);
	//Mux A
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_MUX_A,GPIO_PIN_MUX_A,b_tmp);

	//Mux S1
	b_tmp = (vs & 0b10)? TRUE : FALSE;
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_MUX_S1,GPIO_PIN_MUX_S1,b_tmp);

	//Mux S0
	b_tmp = (vs & 0b1)? TRUE : FALSE;
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_MUX_S0,GPIO_PIN_MUX_S0,b_tmp);
}
