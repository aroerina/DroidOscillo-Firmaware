/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2016
 * Generated linker script file for LPC4370
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.0 [Build 647] [2016-07-18]  on 2016/08/18 0:19:18
 */

INCLUDE "USB_Scope_BOOT_M4_Debug_library.ld"
INCLUDE "USB_Scope_BOOT_M4_Debug_memory.ld"

ENTRY(ResetISR)

SECTIONS
{
  
    
    /* _core_m0app text section */
    .text.core_m0app_text : SUBALIGN(4)
    {
       FILL(0xff)
       __core_m0app_START__ = .; /* start of slave image */
       KEEP(*(.core_m0app))
    }> Flash_M0

    /* _core_m0app extab and exidx sections */
    .text.core_m0app_ARM_extab . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.ARM.extab))
    } 

    .text.core_m0app_ARM_exidx . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.ARM.exidx))
    } 

    /* _core_m0app data section */
    .text.core_m0app_data . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.data_*)) KEEP(*(.core_m0app.data))
        __core_m0app_END__ = .; /* end of slave image */

        /* perform some simple sanity checks */
        ASSERT(!(__core_m0app_START__ == __core_m0app_END__), "No slave code for _core_m0app");
        ASSERT( (ABSOLUTE(__core_m0app_START__) == __vectors_start___core_m0app), "M0APP execute address differs from address provided in source image");
    } 
    .text_Flash2 : ALIGN(4)
    {
       FILL(0xff)
    	*(.text_Flash2*) /* for compatibility with previous releases */
    	*(.text_Flash_M0*) /* for compatibility with previous releases */
    	*(.text.$Flash2*)
    	*(.text.$Flash_M0*)
    	*(.rodata.$Flash2*)
    	*(.rodata.$Flash_M0*)
    } > Flash_M0

    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ; 
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
	    /* End of Global Section Table */

        *(.after_vectors*)

    } >Flash_M4

    .text : ALIGN(4)    
    {
        *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > Flash_M4
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4) 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > Flash_M4
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > Flash_M4
    __exidx_end = .;

    _etext = .;
        
    /* MAIN DATA SECTION */
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > M4_BOOTRAM
    /* Main DATA section (M4_BOOTRAM) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > M4_BOOTRAM AT>Flash_M4
    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > M4_BOOTRAM
    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*) 
         . = ALIGN(4) ;
        _end_noinit = .;
    } > M4_BOOTRAM

    PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .);
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_M4_BOOTRAM - 0);

    /* ## Create checksum value (used in startup) ## */
    PROVIDE(__valid_user_code_checksum = 0 - 
                                         (_vStackTop 
                                         + (ResetISR + 1) 
                                         + (NMI_Handler + 1) 
                                         + (HardFault_Handler + 1) 
                                         + (( DEFINED(MemManage_Handler) ? MemManage_Handler : 0 ) + 1)   /* MemManage_Handler may not be defined */
                                         + (( DEFINED(BusFault_Handler) ? BusFault_Handler : 0 ) + 1)     /* BusFault_Handler may not be defined */
                                         + (( DEFINED(UsageFault_Handler) ? UsageFault_Handler : 0 ) + 1) /* UsageFault_Handler may not be defined */
                                         ) );
}