
 //
 //		main loop
 //
#include "board.h"
#include <scope_main.h>
//#include "lpc_types.h"
//#include "LPC43xx.h"
//#include "lpc43xx_gpio.h"
#include "boot_mc_shared_mem.h"
#include <string.h>
#include "mc_shared_mem.h"

#define TICKRATE_HZ1 (1000)	/* 10 ticks per second */
#define GPIO_PORT_LED		0
#define GPIO_PIN_LED		15
//void SysTick_Handler(void)
//{
//	static bool ledstate = false;
//	static int x = 0;
//	x++;
//	if (x > 500) {
//		ledstate = !ledstate;
//		Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED,ledstate); 	// LED OFF
//		x = 0;
//	}
//}


//volatile uint8_t wait_debuger = 1;
int main(void){

	// initialize boot share memory
    memset((void*)0x10000000,0,0x20000); // initialize local SRAM 128KB
    memset((void*)0x10080000,0,0x12000); // initialize local SRAM 72KB
    memset((void*)BOOT_SHARED_MEMORY_ADDR,0,(0x10000-0x700));// AHB SRAM initialize

	SystemCoreClockUpdate();

	// ADCHS クロック設定
    Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_CRYSTAL, 1);				// Crystal
    Chip_Clock_SetBaseClock(CLK_BASE_ADCHS, CLKIN_IDIVE , true, false);
	//while(wait_debuger);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED);		// LED GPIO DIR SET OUT
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT,GPIO_PORT_LED,GPIO_PIN_LED,false); 	// LED OFF
	//SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

#ifdef RELEASE


    // M0APP start
    LPC_RGU->RESET_CTRL1 = 1<<24;				//Reset
    LPC_CCU1->CLKCCU[CLK_M4_M0APP].CFG |= 1;	// Clock Enable
    LPC_CREG->M0APPMEMMAP = M0_BOOTCODE_ADDR;	//M0APP Flash memory address set
    LPC_RGU->RESET_CTRL1 = 0;					//Reset Clear

#endif

#ifdef DEBUG
    scope_main();
#else
    while(MC_EXCODE_INFO->ExCodeLoaded[EXFUNC_M4] == FALSE);
    ((int(*)())MC_EXCODE_INFO->ExfuncAddr[EXFUNC_M4])();	// execute main();
#endif

    return 0;
}

#ifdef  DEBUG
void check_failed(uint8_t *file, uint32_t line){
	while(1);
}
#endif
