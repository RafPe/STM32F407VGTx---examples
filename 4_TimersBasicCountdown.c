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
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_TIM1EN;  // enable SYSCFG for external interrupts & TIM1

	__DSB();								// Data Synchronization Barrier

	TIM1->PSC = 15999;						// Prescaler we want to be using 16Mhz/16000 = 1000Hz = 1ms
	TIM1->ARR = 499;						// How many values we will count
	TIM1->DIER = TIM_DIER_UIE;				// Update Event Interrupt
	TIM1->CR1 = TIM_CR1_CEN;				// Enable & Start timer

	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);		// Set up interrupt handler

	GPIOD->MODER = (1 << 26); 				// PIND to output mode



	while (1)
	{

	} /* while */


} /* main */



void TIM1_UP_TIM10_IRQHandler(void)
{
	if (TIM1->SR & TIM_SR_UIF)
	{
		GPIOD->ODR ^= (1 << 13);	// Blink

		TIM1->SR = ~TIM_SR_UIF;		// clear flag - rc_w0 => Read Clear Write 0
	}
}


