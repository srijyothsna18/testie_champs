#include <stdint.h>

#define SRAM_START	 0x20000000U
#define SRAM_SIZE	(96*1024) //96KB
#define SRAM_END	((SRAM_START)+(SRAM_SIZE))

#define STACK_START	SRAM_END

extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;

extern uint32_t _ebss;
extern uint32_t _sbss;

extern uint32_t _la_data;

//prototype of main
int main(void);

void __libc_init_array(void);
void Reset_Handler(void)	__attribute__((section(".random_section")));
void NMI_Handler(void) 		__attribute__((weak,alias("default_Handler")));
void HardFault_Handler(void) 	__attribute__((weak,alias("default_Handler")));


uint32_t vectors[] __attribute__((section(".isr_vector"))) = {
	STACK_START,
	(uint32_t)&Reset_Handler,
	(uint32_t)&NMI_Handler,
	(uint32_t)&HardFault_Handler,
	
	
};

void default_Handler(void)
{
	while(1);
}

void Reset_Handler(void)
{
	//copy .data section to SRAM
	uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata;

	uint8_t *pDst = (uint8_t*)&_sdata;	//sram
	uint8_t *pSrc = (uint8_t*)&_la_data;	//flash
	for(uint32_t i=0;i<size;i++)
	{
		*pDst++ = *pSrc++;
	}
	//init. the .bss section to zero in SRAM
	size = (uint32_t)&_ebss - (uint32_t)&_sbss;
	pDst = (uint8_t*)&_sbss;
	for(uint32_t i=0;i<size;i++)
	{
		*pDst++ = 0;
	}

	__libc_init_array();

	main();
}

