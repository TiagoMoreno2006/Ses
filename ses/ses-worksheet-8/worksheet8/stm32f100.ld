/*
Linker script for STM32F10x_256K_64K

modified from

http://www.codesourcery.com/archives/arm-gnu/msg02972.html
http://communities.mentor.com/community/cs/archives/arm-gnu/msg02972.html
*/

/*
There will be a link error if there is not this amount of RAM free at the
end.
*/



/* Memory Spaces Definitions */

MEMORY
{
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 256K
}

__ram_start__ = ORIGIN(RAM);
__ram_size__  = LENGTH(RAM);
__ram_end__   = __ram_start__ + __ram_size__;
_estack = __ram_end__;

/* Sections Definitions */

SECTIONS
{

    .text :
    {
        KEEP(*(.isr_vector))            /* Startup code */
        *(.text)                        /* code */
        *(.text.*)                      /* remaining code */
        *(.rodata)                      /* read-only data (constants) */
        *(.rodata.*)
    } >FLASH
    
    . = ALIGN(4);
     _etext = .;
    /* This is used by the startup in order to initialize the .data secion */
    _sidata = _etext;

    /* This is the initialized data section
    The program executes knowing that the data is in the RAM
    but the loader puts the initial values in the FLASH (inidata).
    It is one task of the startup to copy the initial values from FLASH to
    RAM. */
    .data  : AT ( _sidata )
    {
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data
           secion */
        _sdata = . ;

        *(.data)
        *(.data.*)

        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .data
           secion */
        _edata = . ;
    } >RAM


    /* This is the uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss
           secion */
        _sbss = .;
	      __bss_start__ = _sbss;
        *(.bss)
        *(.bss.*)
        *(COMMON)

        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss
           secion */
        _ebss = . ;
	      __bss_end__ = _ebss;
    } >RAM

    PROVIDE ( _exit = _ebss );
}
