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


	__DSB();

	GPIOD->MODER = (1 << 26); // set pin 13 to be general purpose output

	while (1)
	{
		for (delay = 1000000; delay; delay--)
		{
			GPIOD->ODR ^= (1 << 13);
		}
	}
}

