/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2017
 * Generated linker script file for LPC4370
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on 2017/04/16 17:58:33
 */

MEMORY
{
  /* Define each memory region */
  Flash_M4 (rx) : ORIGIN = 0x14000000, LENGTH = 0x10000 /* 64K bytes (alias Flash) */  
  Flash_M0 (rx) : ORIGIN = 0x14010000, LENGTH = 0x10000 /* 64K bytes (alias Flash2) */  
  M4_BOOTRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x700 /* 1792 bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
  __base_Flash_M4 = 0x14000000  ; /* Flash_M4 */  
  __base_Flash = 0x14000000 ; /* Flash */  
  __top_Flash_M4 = 0x14000000 + 0x10000 ; /* 64K bytes */  
  __top_Flash = 0x14000000 + 0x10000 ; /* 64K bytes */  
  __base_Flash_M0 = 0x14010000  ; /* Flash_M0 */  
  __base_Flash2 = 0x14010000 ; /* Flash2 */  
  __top_Flash_M0 = 0x14010000 + 0x10000 ; /* 64K bytes */  
  __top_Flash2 = 0x14010000 + 0x10000 ; /* 64K bytes */  
  __base_M4_BOOTRAM = 0x20000000  ; /* M4_BOOTRAM */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_M4_BOOTRAM = 0x20000000 + 0x700 ; /* 1792 bytes */  
  __top_RAM = 0x20000000 + 0x700 ; /* 1792 bytes */  
