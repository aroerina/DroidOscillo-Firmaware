#include <vector_table_cmsis.h>
#include "LPC43XX.h"

#ifdef CORE_M4
#define SIZE_OF_VT (69*4)
#endif
#ifdef CORE_M0
#define SIZE_OF_VT (48*4)
#endif


// Copy Vector Table to New Address
void set_new_vtable(uint32_t vt_addr,uint32_t new_vt_addr){
	uint8_t i = 0;
	uint32_t tmp = new_vt_addr;

	for(;i < (SIZE_OF_VT/4);i++){
		*(uint32_t*)new_vt_addr = *(uint32_t*)vt_addr;
		new_vt_addr+=4;
		vt_addr+=4;
	}

#ifdef CORE_M4
	SCB->VTOR = tmp;
#endif
#ifdef CORE_M0
	LPC_CREG->M0APPMEMMAP = tmp;
#endif

}

// Set new IRQ Handler
void set_handler(
		uint32_t vt_addr,
#ifdef CORE_M4
		IRQn_Type irq_number,
#endif
#ifdef CORE_M0
		IRQn_Type irq_number,
#endif
		void (*handler)()){

	irq_number += 16;
	*((uint32_t*)(vt_addr+irq_number*4)) = (uint32_t)handler;
}
