/*
 * main.c
 *
 *  Created on: 12 Feb 2017
 *      Author: rafpe
 */

#include "stm32f4xx.h"
#include "stm32f407xx.h"

#define OV7670_ADDRESS   0x21
#define OV7670_ADDRESS_W 0x42
#define OV7670_ADDRESS_R 0x43


/*
 *	PB6 => I2C1_SCL
 *	PB7 => I2C1_SDA
 */

// SCCB Data / I2C SDA
#define SCCB_DATA_LOW   		GPIOB->BSRR = GPIO_BSRR_BR_7;
#define SCCB_DATA_HIGH 			GPIOB->BSRR = GPIO_BSRR_BS_7;
#define SCCB_DATA_STATUS	 	GPIOB->IDR & GPIO_IDR_IDR_7

// SCCB CLOCK / I2C Clock
#define SCCB_CLOCK_LOW  		GPIOB->BSRR = GPIO_BSRR_BR_6;
#define SCCB_CLOCK_HIGH 		GPIOB->BSRR = GPIO_BSRR_BS_6;


uint32_t reg1,reg2;
volatile uint32_t delay100_us;

void rcc_init(void);
void mc02_init(void);
void gpio_setup(void);
void delayus_100(uint32_t delay);
static void SCCB_Start(void);
static void SCCB_Stop(void);
static void NACK(void);
void SCCB_Write(uint8_t data);
uint8_t SCCB_Read(void);


int main(void)
{

	rcc_init();		// Peripherial init
	mc02_init();	// SYSCLK to PC9
	gpio_setup();

	SysTick_Config(1600);


	// Create start condition on SCCB/I2C interface
	SCCB_Start();
	// Write data (Address of slave device for Write) on SCCB/I2C interface
	SCCB_Write(OV7670_ADDRESS_W);
	delayus_100(1);
	// Write data (Address of register in Camera Module)on SCCB/I2C interface
	SCCB_Write(0x6B);

	// Create stop condition on SCCB/I2C interface
	SCCB_Stop();

	// Delay for SCCB/I2C
	delayus_100(1);
	// Create start condition on SCCB/I2C interface
	SCCB_Start();
	// Write data (Address of slave device for Read) on SCCB/I2C interface
	SCCB_Write(OV7670_ADDRESS_R);
	delayus_100(5);
	// Received data from Camera Module (SCCB/I2C)
	uint8_t test = SCCB_Read();
	// No acknowlage on SCCB/I2C interface
	NACK();
	// Create stop condition on SCCB/I2C interface
	SCCB_Stop();

	while(1)
	{



	}
}

void SysTick_Handler(void)
{
	if(delay100_us > 0) delay100_us--;
}

void rcc_init(void)
{

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN|	// Enable clock: GPIOB
					RCC_AHB1ENR_GPIOCEN;	// Enable clock: GPIOC
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;		// Enable clock: I2C1

	__DSB();	// Data Synchronization Barrier

}
void mc02_init(void)
{
	/*
	 *  Enable SYSCLK output to PC9
	 */
	GPIOC->MODER |= GPIO_MODER_MODER9_1 ;   // AF: PC9 => MCO2
}
void gpio_setup(void)
{
	GPIOB->MODER |= GPIO_MODER_MODER6_0 |   // OUTPUT: PB6 => I2C1_SCL
					GPIO_MODER_MODER7_0; 	// OUTPUT: PB7 => I2C1_SDA


	GPIOB->OTYPER |= GPIO_OTYPER_OT_6|
					 GPIO_OTYPER_OT_7;

	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6|
					  GPIO_OSPEEDER_OSPEEDR7;

    // GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_0|
    //            GPIO_PUPDR_PUPDR7_0;

}
void delayus_100(uint32_t delay)
{
	delay100_us = delay;
	while (delay100_us)
	{

	}
}


static void SCCB_Start(void)
{
	// Configure SIO_D of SCCB/I2C interface as output for write
	GPIOB->MODER |= GPIO_MODER_MODER7_0; 	// OUTPUT: PB7 => I2C1_SDA

	SCCB_DATA_HIGH;
	delayus_100(5);

	SCCB_CLOCK_HIGH;
	delayus_100(5);

	SCCB_DATA_LOW;
	delayus_100(5);

	SCCB_CLOCK_LOW;
	delayus_100(5);
}

static void SCCB_Stop(void)
{
	// Configure SIO_D of SCCB/I2C interface as output for write
	GPIOB->MODER |= GPIO_MODER_MODER7_0; 	// OUTPUT: PB7 => I2C1_SDA

	SCCB_DATA_LOW;
	delayus_100(5);

	SCCB_CLOCK_HIGH;
	delayus_100(5);

	SCCB_DATA_HIGH;
	delayus_100(5);
}

void SCCB_Write(uint8_t data)
{
	uint8_t i;

	// Configure SIO_D of SCCB/I2C interface as output for write
	GPIOB->MODER |= GPIO_MODER_MODER7_0; 	// OUTPUT: PB7 => I2C1_SDA

	// Write data bit by bit on SCCB/I2C
	for(i=0;i<8;i++)
	{
		if((data & 0x80) == 0x80)		// If bit in Data is high, write high on SCCB/I2C
		{
			SCCB_DATA_HIGH;
		}
		else							// If bit in Data is low, write low on SCCB/I2C
		{
			SCCB_DATA_LOW;
		}
		data <<= 1;						// Rotate Data for write next bit

		// Create clock pulse on SCCB/I2C
		// ___  On SIO_C pin (SCCB clock)
		//    \___
		SCCB_CLOCK_HIGH;
		delayus_100(5);
		SCCB_CLOCK_LOW;
		delayus_100(5);
	}

	// Read acknowladge from Camera Module to confirm received data
	delayus_100(1);

	// Configure SIO_D of SCCB/I2C interface as input for read;
	GPIOB->MODER &= ~GPIO_MODER_MODER7; 	// INPUT: PB7 => I2C1_SDA
	delayus_100(5);

	SCCB_CLOCK_HIGH;
	delayus_100(5);

	// If acknowladge is OK return SUCCESS else if is incorrect return ERROR
	uint8_t nasz_ack = GPIOB->IDR & GPIO_IDR_IDR_7;

	// Pulse on SCCB/I2C fall down from high
	SCCB_CLOCK_LOW;
	delayus_100(5);
	// Configure SIO_D of SCCB/I2C interface back to output for write
	GPIOB->MODER |= GPIO_MODER_MODER7_0; 	// OUTPUT: PB7 => I2C1_SDA

//	return (Ack);
}

uint8_t SCCB_Read(void)
{
	uint8_t Data, i;

	// Write to Data zero for correct return data
	Data = 0;
	// Configure SIO_D of SCCB/I2C interface as input for read;
	GPIOB->MODER &= ~GPIO_MODER_MODER7; 	// INPUT: PB7 => I2C1_SDA
	// Delay for  SCCB/I2C interface
	delayus_100(5);
	// Read data from SCCBI/I2C interface
	for(i=8;i>0;i--)
	{
		delayus_100(5);							// Delay for  SCCB/I2C
		SCCB_CLOCK_HIGH;						// Clock high
		delayus_100(5);
		Data = Data << 1;						// Rotate Data << 1
		if(SCCB_DATA_STATUS)
		{
			Data = Data + 1;					// Read DATA pin value
		}
		SCCB_CLOCK_LOW;							// Clock low on SIO_C
		delayus_100(5);
	}
	// Return received data from SCCBI/I2C interface
	return(Data);
}

//
static void NACK(void)
{
	// Configure SIO_D of SCCB/I2C interface back to output for write
	GPIOB->MODER |= GPIO_MODER_MODER7_0; 	// OUTPUT: PB7 => I2C1_SDA

	SCCB_DATA_HIGH;;
	delayus_100(5);

	SCCB_CLOCK_HIGH;
	delayus_100(5);

	SCCB_CLOCK_LOW;
	delayus_100(5);

	SCCB_DATA_LOW;
	delayus_100(5);
}


