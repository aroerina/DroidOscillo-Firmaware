//#include <vector_table_cmsis.h>
#include "scope_init.h"
#include "LPC43xx.h"
#include "lpc43xx_timer.h"
#include "lpc43xx_gpio.h"
#include "lpc43xx_cgu.h"
#include "lpc43xx_scu.h"
//#include "system_LPC43xx.h"

#include "vector_table_cmsis.h"
#include "boot_mc_shared_mem.h"
#include "mc_shared_mem.h"
#include "scope_main.h"

#define TESTSIG_ENABLE
#define DESIRED_TESTSIG_FREQ	10000	// 10khz

volatile GPDMA_LLI_Type adc_first_lli ,adc_second_lli ,adc_third_lli ,adc_4th_lli ,lli_role_view,auto_free_lli;
volatile buffer_t buffer __attribute__ ((section(".bss.$RAM2")));

volatile uint8_t wait_debugger = 0;

void M0CORE_IRQHandler(void);

__attribute__ ((section(".isr_vector")))
volatile uint32_t pseudo_vectortable[69];	//ベクターテーブル配置用空メモリ
__attribute__ ((section(".after_vectors")))
void ResetISR(void) {		// Entry Point

	while(wait_debugger);

	// Disable peripheral clocks
	LPC_CCU1->CLK_M4_EMCDIV_CFG = 0;
	LPC_CCU1->CLK_M4_RITIMER_CFG = 0;
	LPC_CCU1->CLK_M4_USART0_CFG = 0;
	LPC_CCU1->CLK_M4_USART2_CFG = 0;
	LPC_CCU1->CLK_M4_USART3_CFG = 0;
	LPC_CCU1->CLK_M4_SDIO_CFG = 0;
	LPC_CCU1->CLK_PERIPH_BUS_CFG = 0;
	LPC_CCU1->CLK_PERIPH_SGPIO_CFG = 0;
	LPC_CCU1->CLK_PERIPH_CORE_CFG = 0;	// M0Sub disable
	LPC_CCU1->CLK_APB3_BUS_CFG = 0;

	LPC_CCU2->CLK_APB0_USART0_CFG = 0;	// disable
	LPC_CCU2->CLK_APB2_USART2_CFG = 0;
	LPC_CCU2->CLK_APB2_USART3_CFG = 0;
	LPC_CCU2->CLK_APB0_SSP0_CFG = 0;
	LPC_CCU2->CLK_APB2_SSP1_CFG = 0;
	LPC_CCU2->CLK_APB0_UART1_CFG = 0;

	LPC_CCU2->CLK_SDIO_CFG = 0;


	CGU_ConfigPWR(CGU_PERIPHERAL_ADC0,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_ADC1,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_AES,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_APB3_BUS,DISABLE);

//	CGU_ConfigPWR(,DISABLE);

	CGU_ConfigPWR(CGU_PERIPHERAL_CAN,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_DAC,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_EMC,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_ETHERNET,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_ETHERNET_TX,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_I2C1,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_I2S,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_LCD,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_MOTOCON,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_QEI,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_RITIMER,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_SCT,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_SDIO,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_SSP0,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_SSP1,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_UART0,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_UART1,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_UART2,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_UART3,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_USB1,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_WWDT,DISABLE);
	CGU_ConfigPWR(CGU_PERIPHERAL_EEPROM,DISABLE);

	// DISABLE Entity Clocks
	//CGU_EnableEntity(,DISABLE);
	CGU_EnableEntity(CGU_CLKSRC_32KHZ_OSC,DISABLE);
	CGU_EnableEntity(CGU_CLKSRC_IRC,DISABLE);
	CGU_EnableEntity(CGU_CLKSRC_ENET_RX_CLK,DISABLE);
	CGU_EnableEntity(CGU_CLKSRC_ENET_TX_CLK,DISABLE);
	CGU_EnableEntity(CGU_CLKSRC_GP_CLKIN,DISABLE);
	CGU_EnableEntity(CGU_BASE_SAFE,DISABLE);
	CGU_EnableEntity(CGU_BASE_USB1,DISABLE);
	CGU_EnableEntity(CGU_BASE_PHY_RX,DISABLE);
	CGU_EnableEntity(CGU_BASE_PHY_TX,DISABLE);
	CGU_EnableEntity(CGU_BASE_APB3,DISABLE);
	CGU_EnableEntity(CGU_BASE_LCD,DISABLE);
	CGU_EnableEntity(CGU_BASE_SDIO,DISABLE);
	CGU_EnableEntity(CGU_BASE_SSP0,DISABLE);
	CGU_EnableEntity(CGU_BASE_SSP1,DISABLE);
	CGU_EnableEntity(CGU_BASE_UART0,DISABLE);
	CGU_EnableEntity(CGU_BASE_UART1,DISABLE);
	CGU_EnableEntity(CGU_BASE_UART2,DISABLE);
	CGU_EnableEntity(CGU_BASE_UART3,DISABLE);
	CGU_EnableEntity(CGU_BASE_CLKOUT,DISABLE);
	CGU_EnableEntity(CGU_BASE_OUT0,DISABLE);
	CGU_EnableEntity(CGU_BASE_OUT1,DISABLE);
	CGU_EnableEntity(CGU_BASE_PERIPH_CLK,DISABLE);
	CGU_EnableEntity(CGU_BASE_SPI,DISABLE);


    //
    //	PLL0 Setup
    //

    // デバッグメモ
    // USBPLL -> DIVA(3) -> DIVC(2) ->ADC ：FLASH読み込みでコケる デバッグモードだと作動
    // AUDIOPLL -> DIVC(6)-> ADC ：表示が激しく乱れる デバッグモードでもコケない
    // AUDIOPLL -> DIVA(3)-> DIVC(6) -> ADC ： USB認識、作動せず
    // USBPLLのクロックは一度IDIVAに通してからでないとIDIVEに通せない

    // USB用PLLを流用すると何故かデバッグでコケるのでAUDIO用を使う
	LPC_CGU->PLL0AUDIO_CTRL |= 1; 	// Power down PLL
						//	P			N
	LPC_CGU->PLL0AUDIO_NP_DIV = (98<<0) | (514<<12);
						//	SELP	SELI	SELR	MDEC
	LPC_CGU->PLL0AUDIO_MDIV = (0xB<<17)|(0x10<<22)|(0<<28)|(0x7FFA<<0);
	LPC_CGU->PLL0AUDIO_CTRL = (CGU_CLKSRC_XTAL_OSC<<24)|(1<<14)|(1<<13)|(1<<11)|(1<<4)|(1<<2)|(1<<3)|(0<<0);

	CGU_SetDIV(CGU_CLKSRC_IDIVE,6);		// Set DIVE divisor 480/6 = 80
	CGU_EntityConnect(CGU_CLKSRC_PLL0_AUDIO,CGU_CLKSRC_IDIVE);
	CGU_EntityConnect(CGU_CLKSRC_IDIVE,CGU_BASE_ADCHS);

//	CGU_SetDIV(CGU_CLKSRC_IDIVE,20);		// Set DIVE divisor 480/6 = 80
//	CGU_EntityConnect(CGU_CLKSRC_PLL1,CGU_CLKSRC_IDIVE);
//	CGU_EntityConnect(CGU_CLKSRC_IDIVE,CGU_BASE_ADCHS);


    // ADCHSのクロックにIDIVE + PLL0USB を使う	　→	表示が乱れてダメ
//    CGU_SetDIV(CGU_CLKSRC_IDIVE,6);		// Set DIVE divisor 480/6 = 80
//    //CGU_SetDIV(CGU_CLKSRC_IDIVA,1);		// Set DIVA divisor = 1
//    CGU_EntityConnect(CGU_CLKSRC_PLL0,CGU_CLKSRC_IDIVE);
//    //CGU_EntityConnect(CGU_CLKSRC_IDIVA,CGU_CLKSRC_IDIVE);
//    CGU_EntityConnect(CGU_CLKSRC_IDIVE,CGU_BASE_ADCHS);		// ADCHS Clock source is DIVE

	MCV->RunningMode = RUNMODE_STOP;
    SystemCoreClock = 204000000;

	// 初期トリガー
	MCV->VTrigger = 2048;
	MCV->HTrigger = 0;
	MCV->SampleLength = 800;

    //ADCHS DMA Setup

	GPDMA_Channel_CFG_Type dmacfg;

    dmacfg.ChannelNum = DMACH_ADCHS;
    dmacfg.TransferSize = ADC_DMA_SIZE;
    dmacfg.TransferWidth = GPDMA_WIDTH_WORD; //32bit で転送
    dmacfg.TransferType = GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA;
    dmacfg.SrcConn = GPDMA_CONN_ADCHS_RD;
    dmacfg.SrcMemAddr = (uint32_t)LPC_ADCHS->FIFO_OUTPUT;
    dmacfg.DstMemAddr = (uint32_t)buffer.mem_W; // 32bit 区切りで指定する
    dmacfg.DMALLI = (uint32_t)&adc_second_lli;
    GPDMA_Setup(&dmacfg);

    //LPC_GPDMA->C0CONFIG = LPC_GPDMA->C0CONFIG&(~GPDMA_DMACCxConfig_ITC);	// ADCHS Channel GPDMA Interrupt Disable
	LPC_GPDMA->INTTCCLEAR = -1;	// Terminal Count Interrupt flag clear
	LPC_GPDMA->INTERRCLR = -1;	// Error Interrupt flag clear

    //GPDMA_ChannelCmd(0,ENABLE);		//DMA Channel0 Enable


    //
    //	DMA Linked list definitions
    //

    // 最初に読み込まれるLLI(二回目からは不要)
    // Channel Interrupt がENABLEになってる
    adc_first_lli.SrcAddr = dmacfg.SrcMemAddr;
    adc_first_lli.DstAddr = dmacfg.DstMemAddr;
    adc_first_lli.Control = DMA_SET_TRANSFER_SIZE(LPC_GPDMA->C0CONTROL,SAMPLE_SIZE_WORD);
    adc_first_lli.NextLLI = (uint32_t)&adc_second_lli;

    // トリガ位置までに最低限必要なサンプル数まで達したら読み込まれるLLI
    adc_second_lli.SrcAddr = dmacfg.SrcMemAddr;
    adc_second_lli.DstAddr = dmacfg.DstMemAddr + SAMPLE_SIZE_BYTE;
    adc_second_lli.Control = DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,BUFFER_SIZE_WORD-SAMPLE_SIZE_WORD);	//	Terminal Count Interrupt Disable
    adc_second_lli.NextLLI = (uint32_t)&adc_third_lli;

    // バッファが一度、一杯になったら読み込まれるLLI
    adc_third_lli = adc_first_lli;
    adc_third_lli.Control = DMA_SET_TRANSFER_SIZE(adc_first_lli.Control,BUFFER_SIZE_WORD);		// バッファ全体
    adc_third_lli.NextLLI = (uint32_t) &adc_4th_lli;

    // 現在読み込まれているLLIをNextLLIから判別するためもう一つ用意	中身は3rd_lliと同じ
    adc_4th_lli = adc_third_lli;

    // ロールビューモード用LLI
    lli_role_view.SrcAddr = dmacfg.SrcMemAddr;
    lli_role_view.DstAddr = (uint32_t) MCV->Buffer;
    lli_role_view.Control = DMA_SET_TRANSFER_SIZE(LPC_GPDMA->C0CONTROL,SAMPLE_SIZE_WORD);
    lli_role_view.NextLLI = (uint32_t)&lli_role_view;

    // オートフリー用LLI 水平トリガーが画面内、もしくは画面外右側の時に使用する
    auto_free_lli.SrcAddr = dmacfg.SrcMemAddr;
    auto_free_lli.NextLLI = (uint32_t)&adc_second_lli;



    MCV->BufferIsPending = FALSE;
    MCV->RunningMode = RUNMODE_STOP;

    //
    //	Timer configuration
    //
    // TIMER0 : 16ms sample transmit timer (M0APP)
    // TIMER1 : ADC stop timer (M4)
    // TIMER2 : AUTO MODE timer (M4)
    // TIMER3 : Probe adjust signal generation (M4)

    //
    // TIMER1 ADC Stop timer config
    //

    TIM_TIMERCFG_Type tconfig;
    tconfig.PrescaleOption = TIM_PRESCALE_TICKVAL;
    tconfig.PrescaleValue = 1;
    TIM_Init(ADC_STOP_TIMER,TIM_TIMER_MODE,&tconfig);

    TIM_MATCHCFG_Type mconfig;
    mconfig.MatchChannel = 0;
    mconfig.IntOnMatch = ENABLE;
    mconfig.StopOnMatch = ENABLE;
    mconfig.ResetOnMatch = ENABLE;
    mconfig.MatchValue = 1000;
    mconfig.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    TIM_ConfigMatch(ADC_STOP_TIMER,&mconfig);
    ADC_STOP_TIMER->IR = 0x1;

    //
    // TIMER2 AUTO MODE Timer
    //

    tconfig.PrescaleOption = TIM_PRESCALE_TICKVAL;
    tconfig.PrescaleValue = SystemCoreClock / 1000;		// count up per 1ms
    TIM_Init(AUTO_MODE_TIMER,TIM_TIMER_MODE,&tconfig);


    mconfig.MatchChannel = 0;
    mconfig.IntOnMatch = ENABLE;
    mconfig.StopOnMatch = ENABLE;
    mconfig.ResetOnMatch = ENABLE;
    mconfig.MatchValue = 1000;		// * 1ms
    mconfig.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    TIM_ConfigMatch(AUTO_MODE_TIMER,&mconfig);
    AUTO_MODE_TIMER->IR = 0x1;


#ifdef TESTSIG_ENABLE
    //
    // Probe adjust signal output setting	TIMER3
    //

     tconfig.PrescaleOption = TIM_PRESCALE_TICKVAL;
     tconfig.PrescaleValue = 1;
     TIM_Init(TEST_SIGNAL_TIMER,TIM_TIMER_MODE,&tconfig);

     mconfig.MatchChannel = 0;
     mconfig.IntOnMatch = DISABLE;
     mconfig.StopOnMatch = DISABLE;
     mconfig.ResetOnMatch = ENABLE;
     mconfig.MatchValue = SystemCoreClock/(DESIRED_TESTSIG_FREQ*2);
     mconfig.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
     TIM_ConfigMatch(TEST_SIGNAL_TIMER,&mconfig);

     //LPC4370 TFBGA100 TIMER3 MATCH0 OUTPUT = P2_3 (D8)

    scu_pinmux(2,3,SLEWRATE_FAST,FUNC6);
    LPC_TIMER3->TCR = 1; // timer enable
#endif


#ifdef DEBUG
    // M0APP start
    LPC_RGU->RESET_CTRL1 = 1<<24;	//Reset
    LPC_CCU1->CLK_M4_M0APP_CFG |= 1;	// Clock Enable
    LPC_CREG->M0APPMEMMAP = M0_FLASH_ADDR;					//M0APP Flash memory address set
    LPC_RGU->RESET_CTRL1 = 0;	//Reset Clear
#endif

    //
    // interrupt setting
    //

	set_new_vtable(M4_BOOTCODE_ADDR,M4_EXCODE_ADDR);
	set_handler(M4_EXCODE_ADDR,M0CORE_IRQn,M0CORE_IRQHandler);	// register IPC M0 handler
	set_handler(M4_EXCODE_ADDR,TIMER2_IRQn,auto_timer_handler);	// register TIMER2 handler

    //IPC Interrupt setting
    NVIC_EnableIRQ(M0CORE_IRQn);
    NVIC_SetPriority(M0CORE_IRQn,100);

    //Timer2 interrupt Enable Setting
    NVIC_SetPriority(TIMER2_IRQn,13);
    NVIC_EnableIRQ(TIMER2_IRQn);

    //
    //	ADCHS Register Setup
    //

    // ！！ADCHSのレジスタにはBASE_ADCHS_CLKを設定する前にアクセスするとCPUが停止する！！

    LPC_ADCHS->ADC_SPEED = 0xEEEEEE;							// Max speed
    LPC_ADCHS->CONFIG = (0x90<<6)|(1<<0);						// software trigger only
    LPC_ADCHS->DESCRIPTOR[0][0] = (1<<31)|(1<<24)|(0b01<<22)|(0x0<<8)|(0b01<<6)|(0<<0);			// use DESCRIPTOR0 only Threshold A Select

    // ADCストップ用 MATCH_VALUEを1以上にしないと止まらない
    LPC_ADCHS->DESCRIPTOR[1][0] = (1<<31)|(1<<24)|(0b01<<22)|(1<<8)|(0b00<<6)|(1<<3)|(0<<0);	// Power downをONにしても何故かパワーダウンレジスタがONしない
    LPC_ADCHS->FIFO_CFG = (8<<1)|(0x1<<0);					// FIFO Fill level = 8 then DMA raise ; Packed Read ON

    // Threshold Interrupt Setting
    LPC_ADCHS->THR[0] = (0<<16)|(MCV->VTrigger<<0);	// Threshold A Set
    LPC_ADCHS->INTS[1].SET_EN = ADC_INT1_THRESHOLD;		//Set Interrupt Mask threshold upper cross
    LPC_ADCHS->INTS[1].CLR_STAT = -1;
    LPC_ADCHS->POWER_CONTROL = (0x3<<17)|(0x4);		// bandgap on.DCINNEG off,DCINPOS off, Max power

    LPC_ADCHS->DSCR_STS = 0;	//debug
	LPC_ADCHS->INTS[1].CLR_EN 	= -1;		// ADCHS Interrupt disable
	LPC_ADCHS->INTS[1].CLR_STAT = -1;		// 割り込みフラグクリア
    LPC_ADCHS->TRIGGER = 1;

    scope_main();
}

void M0CORE_IRQHandler(void){
	LPC_CREG->M0TXEVENT = 0;	// interrupt flag clear
}

void start_adc(void){

	//LPC_ADCHS->POWER_CONTROL |= 1<<17;	// Power switch ON
	//LPC_ADCHS->POWER_DOWN = 0;	// ADC Power ON
	LPC_ADCHS->DSCR_STS = 0;	// change to descriptor0
	uint32_t i=0;
	for(i=0;i<100;i++){			// 変換し始めのサンプルは波形が歪むので捨てる
		LPC_ADCHS->FLUSH = 1;		// FIFO FLUSH
	}
	//LPC_GPDMA->CONFIG = 1;
	LPC_GPDMA->C0CONFIG |= GPDMA_DMACCxConfig_E;	// start DMA
	if(LPC_ADCHS->DESCRIPTOR[0][0]&0x3fff00)LPC_ADCHS->TRIGGER = 1;		// Set Trigger if descriptor0 MATCH_VALUE>0
}

inline void stop_adc(void){
	//LPC_ADCHS->POWER_CONTROL &= ~(1<<17);	// Power switch OFF
	//LPC_ADCHS->POWER_DOWN = 1;	// ADC Power down
	LPC_GPDMA->C0CONFIG &= ~GPDMA_DMACCxConfig_E;	// stop DMA
	//LPC_GPDMA->CONFIG = 0;
	LPC_ADCHS->DSCR_STS = 1;	// change descriptor table for stop adc

}

#ifdef RELEASE
void check_failed(uint8_t *file, uint32_t line)
{
while(1);
}
#endif
