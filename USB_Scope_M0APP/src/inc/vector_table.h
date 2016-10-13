/*
 * vector_table.h
 *
 *  Created on: 2016/03/16
 *      Author: AROE
 */

#ifndef SRC_VECTOR_TABLE_H_
#define SRC_VECTOR_TABLE_H_

#include "cmsis.h"

void set_new_vtable(uint32_t vt_addr,uint32_t new_vt_addr);


void set_handler(
		uint32_t vt_addr,
#ifdef CORE_M4
		LPC43XX_IRQn_Type irq_number,
#endif
#ifdef CORE_M0
		LPC43XX_M0_IRQn_Type irq_number,
#endif
		void (*handler)());

#endif /* SRC_VECTOR_TABLE_H_ */
