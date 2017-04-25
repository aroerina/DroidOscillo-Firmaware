/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2017
 * Generated linker script file for LPC4370
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on 2017/04/21 14:32:51
 */

MEMORY
{
  /* Define each memory region */
  M4_MAINRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x10000 /* 64K bytes (alias RAM) */  
  AHBRAM16KB (rwx) : ORIGIN = 0x20004000, LENGTH = 0x4000 /* 16K bytes (alias RAM2) */  
}

  /* Define a symbol for the top of each memory region */
  __base_M4_MAINRAM = 0x10000000  ; /* M4_MAINRAM */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_M4_MAINRAM = 0x10000000 + 0x10000 ; /* 64K bytes */  
  __top_RAM = 0x10000000 + 0x10000 ; /* 64K bytes */  
  __base_AHBRAM16KB = 0x20004000  ; /* AHBRAM16KB */  
  __base_RAM2 = 0x20004000 ; /* RAM2 */  
  __top_AHBRAM16KB = 0x20004000 + 0x4000 ; /* 16K bytes */  
  __top_RAM2 = 0x20004000 + 0x4000 ; /* 16K bytes */  
