/*
 * main.c
 *
 *  Created on: 2 Nov 2016
 *      Author: rafpe
 */
#include "stm32f4xx.h"
#include "stm32f407xx.h"




int main(void)
{
	volatile uint32_t delay;

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOCEN; 	// enable the clock to GPIOD & GPIOC
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; 						// enable SYSCFG for external interrupts

	__DSB();								// Data Synchronization Barrier

	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI1_PC;	// Interrupt from PC1

	EXTI->FTSR = EXTI_FTSR_TR1; 	// Falling edge
	EXTI->RTSR = EXTI_RTSR_TR1; 	// Rising edge

	EXTI->IMR = EXTI_IMR_MR1;		// Enable interrupt on line 1


	NVIC_EnableIRQ(EXTI1_IRQn);

	GPIOD->MODER = (1 << 26); 		// PIND to output mode



	while (1)
	{

	} /* while */


} /* main */



void EXTI1_IRQHandler(void)
{

	if ( EXTI->PR & EXTI_PR_PR1)
	{
		GPIOD->ODR ^= (1 << 13);	// Blink

		EXTI->PR = EXTI_PR_PR1;		// Handle interrupt
	}

} /* EXTI1_IRQHandler */


