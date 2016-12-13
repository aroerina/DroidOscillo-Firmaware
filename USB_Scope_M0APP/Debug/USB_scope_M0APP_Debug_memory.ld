/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2016
 * Generated linker script file for LPC4370-M0
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.0 [Build 647] [2016-07-18]  on 2016/08/18 8:12:21
 */

MEMORY
{
  /* Define each memory region */
  M0_MAINRAM (rwx) : ORIGIN = 0x10020000, LENGTH = 0x10000 /* 64K bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
  __base_M0_MAINRAM = 0x10020000  ; /* M0_MAINRAM */  
  __base_RAM = 0x10020000 ; /* RAM */  
  __top_M0_MAINRAM = 0x10020000 + 0x10000 ; /* 64K bytes */  
  __top_RAM = 0x10020000 + 0x10000 ; /* 64K bytes */  
