/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2015
 * (c) NXP Semiconductors 2013-2015
 * Linker script memory definitions
 * Created from LinkMemoryTemplate
 * By LPCXpresso v7.6.2 [Build 326] [2015-02-02]  on Wed Dec 30 23:07:16 JST 2015)
*/

MEMORY
{
  /* Define each memory region */
  Flash_M4 (rx) : ORIGIN = 0x14000000, LENGTH = 0x10000 /* 64K bytes */
  Flash_M0 (rx) : ORIGIN = 0x14010000, LENGTH = 0x10000 /* 64K bytes */
  RamLoc128 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x20000 /* 128K bytes */
  RamM0Sub16 (rwx) : ORIGIN = 0x18000000, LENGTH = 0x4000 /* 16K bytes */
  RamM0Sub2 (rwx) : ORIGIN = 0x18004000, LENGTH = 0x800 /* 2K bytes */


}
  /* Define a symbol for the top of each memory region */
  __top_Flash_M4 = 0x14000000 + 0x10000;
  __top_Flash_M0 = 0x14010000 + 0x10000;
  __top_RamLoc128 = 0x10000000 + 0x20000;
  __top_RamM0Sub16 = 0x18000000 + 0x4000;
  __top_RamM0Sub2 = 0x18004000 + 0x800;



