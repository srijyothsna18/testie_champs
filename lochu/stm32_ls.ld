ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

SECTIONS
{
    .text :
    {
	*(.isr_vector)
        *(.text)               /* All text sections from .o files */
	*(.text.*)
	*(.init)
	*(.fini)
        *(.rodata)  		/* Read-only data */
	*(.rodata.*)
	. = ALIGN(4); 
        _etext = .;            /* End of text */
    } > FLASH 

    _la_data = LOADADDR(.data);
    .data :
    {
        _sdata = .;           /* Start of .data section */
        *(.data)        	/* All data sections */
	. = ALIGN(4); 
        _edata = .;           /* End of .data section */
    } > SRAM AT> FLASH

    .bss :
    {
        _sbss = .; 		/* Start of .bss section */
	__bss_start__ = _sbss;       
       	*(.bss) 
	. = ALIGN(4); 
	*(COMMON)
        _ebss = .; 	/* End of .bss section */
	__bss_end__ = _ebss;
	. = ALIGN(4);
	end = .;
    } > SRAM

}

