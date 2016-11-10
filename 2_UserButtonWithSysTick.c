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
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // enable the clock to GPIOD & GPIOA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // enable the clock to GPIOD & GPIOA

	SysTick_Config(8000000);

	__DSB();

	GPIOD->MODER = (1 << 26); // set pin 13 to be general purpose output

	while (1)
	{
		if(GPIOA->IDR & GPIO_IDR_IDR_0)
		{
			GPIOD->ODR ^= (1 << 13);
		}


	}
}

void SysTick_Handler(void)
{
	GPIOD->ODR ^= (1 << 13);
}


