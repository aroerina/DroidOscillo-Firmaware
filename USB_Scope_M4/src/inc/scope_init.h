// 204MHz = 1clk = 0.000,000,004,901 = 4.901ns

#ifndef __USB_SCOPE_INIT_H
#define __USB_SCOPE_INIT_H

#include "lpc43xx_gpdma.h"
#include "mc_shared_mem.h"

#define BUFFER_HW_BW				12		//2^n * 16bit Half word 4096 sample
#define BUFFER_BYTE_BW				(BUFFER_HW_BW+1)
#define SAMPLE_SIZE_BYTE			(MCV_BUFFER_SAMPLE_LENGTH * 2)
#define SAMPLE_SIZE_WORD			(MCV_BUFFER_SAMPLE_LENGTH/2)
#define SAMPLE_OVER_HEAD			((1<<BUFFER_HW_BW)-MCV_BUFFER_SAMPLE_LENGTH)
#define BUFFER_COUNTER_MASK			((1<<BUFFER_HW_BW)-1)
#define BUFFER_ADRESS_MASK			((1<<(BUFFER_HW_BW+1))-1)

#define BUFFER_SIZE_HW				(1<<BUFFER_HW_BW)
#define BUFFER_SIZE_BYTE			(BUFFER_SIZE_HW*2)	// byte
#define BUFFER_SIZE_WORD			(BUFFER_SIZE_BYTE/4)
#define ADC_DMA_SIZE				(BUFFER_SIZE_BYTE/4)		// word

#define DMACH_ADCHS		0
#define DMA_TRANSFER_SIZE_BM	(~0xfff)
#define DMA_SET_TRANSFER_SIZE(control,size)	(((control) & DMA_TRANSFER_SIZE_BM) | (size))

#define SAMPLE_CLOCK_DIVIDE			2.55			//ADCサンプリング用分周値 3~255　まで

#define ADC_INT1_THRESHOLD	(0x1<<3)

#define DMA_ADCCH_STOP		LPC_GPDMA->C0CONFIG &= ~GPDMA_DMACCxConfig_E
#define DMA_ADCCH_RUN		LPC_GPDMA->C0CONFIG |= GPDMA_DMACCxConfig_E

#define MESSAGE_NO_TRIGGER		'N'
#define MESSAGE_TRIGGERD		'T'

// Timer definition

#define ADC_STOP_TIMER			LPC_TIMER1
#define AUTO_MODE_TIMER			LPC_TIMER2
#define TEST_SIGNAL_TIMER		LPC_TIMER3

#define TIMER_STABILIZE_TICK	60		// +20 sample　@ 68MHz
// トリガーマーカーより実際のトリガー位置が左側にある場合増やす（早めにサンプリングを終える）
#ifdef DEBUG
#define	TIMER_OVERHEAD_TICK		(300 + TIMER_STABILIZE_TICK)
#define NUM_STABILIZE_SAMPLE 	600		// 探索範囲 * 1/2
#else
#define TIMER_OVERHEAD_TICK		(200 + TIMER_STABILIZE_TICK)
#define NUM_STABILIZE_SAMPLE 	200		// 探索範囲 * 1/2
#endif

typedef union{
	uint32_t mem_W[BUFFER_SIZE_WORD];	// 32bit access
	uint16_t mem_HW[BUFFER_SIZE_HW];	// 16bit access
}buffer_t;

volatile extern buffer_t buffer;
volatile extern GPDMA_LLI_Type adc_first_lli;
volatile extern GPDMA_LLI_Type adc_second_lli;
volatile extern GPDMA_LLI_Type adc_third_lli;
volatile extern GPDMA_LLI_Type adc_4th_lli,lli_role_view,auto_free_lli;

void scope_init(void);
void start_adc(void);
void stop_adc(void);
#endif
