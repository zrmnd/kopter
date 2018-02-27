#ifndef _DWT_H_
#define _DWT_H_

#include "stm32f10x.h" 


//------------------------------------------------------------------------------
#define    DWT_CYCCNT    *(volatile unsigned long *)0xE0001004
#define    DWT_CONTROL   *(volatile unsigned long *)0xE0001000
#define    SCB_DEMCR     *(volatile unsigned long *)0xE000EDFC
	
//------------------------------------------------------------------------------




void initDWT(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);




#endif 
