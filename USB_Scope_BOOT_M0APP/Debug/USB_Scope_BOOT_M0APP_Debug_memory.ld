/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2016
 * Generated linker script file for LPC4370-M0
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.0 [Build 647] [2016-07-18]  on 2016/08/18 8:11:37
 */

MEMORY
{
  /* Define each memory region */
  M0APP_FLASH (rx) : ORIGIN = 0x14010000, LENGTH = 0x10000 /* 64K bytes (alias Flash) */  
  M0_BOOTRAM (rwx) : ORIGIN = 0x20001000, LENGTH = 0x1000 /* 4K bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
  __base_M0APP_FLASH = 0x14010000  ; /* M0APP_FLASH */  
  __base_Flash = 0x14010000 ; /* Flash */  
  __top_M0APP_FLASH = 0x14010000 + 0x10000 ; /* 64K bytes */  
  __top_Flash = 0x14010000 + 0x10000 ; /* 64K bytes */  
  __base_M0_BOOTRAM = 0x20001000  ; /* M0_BOOTRAM */  
  __base_RAM = 0x20001000 ; /* RAM */  
  __top_M0_BOOTRAM = 0x20001000 + 0x1000 ; /* 4K bytes */  
  __top_RAM = 0x20001000 + 0x1000 ; /* 4K bytes */  
