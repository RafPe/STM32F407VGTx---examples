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

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOAEN;	// enable the clock for GPIOs
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_TIM1EN;							// enable SYSCFG for external interrupts & TIM1

	__DSB();																			// Data Synchronization Barrier

	GPIOD->MODER = (1 << 26); 				// PIND to output mode
    GPIOE->MODER = GPIO_MODER_MODER1_0;		// PE1 - output


	/*
	 * We start off with setting this is AF. Once done we map it to AF.
	 * PA8 to be used as alternate function TIM1_CH1
	 *
	 * AF details you always find in datasheet
	 * AF mapping you will find in RM
	 */
	GPIOA->MODER |= GPIO_MODER_MODER8_1; 	// PA8 - AF
	GPIOA->AFR[1] = ( (GPIOA->AFR[1] && 0xFFFFFFF0 ) | 0x1);


	TIM1->CCMR1  = TIM_CCMR1_CC1S_0;													// CC1 channel is configured as output
	TIM1->CCER 	 = TIM_CCER_CC1P |
			       TIM_CCER_CC1NP;														// Set up rising edge polarity

	TIM1->SMCR 	 = TIM_SMCR_TS_0 | TIM_SMCR_TS_2 |										// Filtered Timer Input 1 (TI1FP1)
				   TIM_SMCR_SMS;														// External Clock Mode 1


	TIM1->ARR 	= 10;						// How many values we will count
	TIM1->DIER 	= TIM_DIER_UIE;				// Update Event Interrupt
	TIM1->CR1	= TIM_CR1_CEN;				// Enable & Start timer

	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);		// Set up interrupt handler




	SysTick_Config(1600000);

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

void SysTick_Handler(void)
{
	GPIOE->ODR ^= (1 << 1);		// PE1 - flapp
}

