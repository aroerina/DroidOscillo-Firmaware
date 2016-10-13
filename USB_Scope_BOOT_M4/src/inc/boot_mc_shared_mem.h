#ifndef BOOT_MC_SHARED_MEM_H
#define BOOT_MC_SHARED_MEM_H
/**************************************
 *
	LPC4370 SRAM

	Local SRAM
	0x10000000 - 0x10020000 128KB
	0x1008A000 - 0x10092000 72KB
	total 200KB

	M0 SubSystem SRAM 18KB
	0x18000000 - 0x18004800 18KB

	AHB SRAM 64KB
	0x20000000 - 0x20010000 64KB


	Memory assign

	M4 Boot code -> 14000000	64KB
	M0 Boot code -> 14010000	64KB

	M4 boot memory ->    0x20000000 - 0x20000700 1792 Byte
	boot share memory -> 0x20000700 - 0x20000800 256 Byte
	M0 boot memory ->	 0x20000800 - 0x20001000 2KB
	USBD Stack	   ->    0x20001000 - 0x20002000 4KB

	Multicore Share memory -> 0x2000C000 - 0x20010000 16KB

	USB ROAD CODE
	M4 0x1000000 - 0x10010000
	M0 0x1000000 - 0x10020000

 *
 *
 *
 */

#define M4_BOOTCODE_ADDR	0x14000000
#define M0_BOOTCODE_ADDR	0x14010000


#define BOOT_SHARED_MEMORY_ADDR		0x20000700
#define BOOT_SHARED_MEMORY_SIZE		0x100

typedef struct {
	uint32_t ExCodeStartAddr[3];	// Extra code memory region top address
	uint32_t ExfuncAddr[3];			// Entrypoint address
	Bool ExCodeLoaded[3];
} EXCODE_INFO_T;

#define EXFUNC_M4		0
#define EXFUNC_M0		1
#define EXFUNC M0SUB	2
#define MC_EXCODE_INFO		((EXCODE_INFO_T*)BOOT_SHARED_MEMORY_ADDR)

#endif
