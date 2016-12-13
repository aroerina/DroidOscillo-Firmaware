/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2016
 * Generated linker script file for LPC4370
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.0.0 [Build 526] [2015-11-23]  on 2016/03/28 17:42:13
 */

MEMORY
{
  /* Define each memory region */
  M4_MAINRAM (rwx) : ORIGIN = 0x10000000, LENGTH = 0x10000 /* 64K bytes (alias RAM) */  
  LOCALRAM72 (rwx) : ORIGIN = 0x10080000, LENGTH = 0x13000 /* 76K bytes (alias RAM2) */  
}

  /* Define a symbol for the top of each memory region */
  __base_M4_MAINRAM = 0x10000000  ; /* M4_MAINRAM */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_M4_MAINRAM = 0x10000000 + 0x10000 ; /* 64K bytes */  
  __top_RAM = 0x10000000 + 0x10000 ; /* 64K bytes */  
  __base_LOCALRAM72 = 0x10080000  ; /* LOCALRAM72 */  
  __base_RAM2 = 0x10080000 ; /* RAM2 */  
  __top_LOCALRAM72 = 0x10080000 + 0x13000 ; /* 76K bytes */  
  __top_RAM2 = 0x10080000 + 0x13000 ; /* 76K bytes */  
