/*
 * main.c
 *
 *  Created on: 12 Feb 2017
 *      Author: rafpe
 */

#include "stm32f4xx.h"
#include "stm32f407xx.h"

#define P6_7

#define QT_DEVICE_ID            0x2E    // Chip ID (read from device-address 0)
#define QT_I2C_ADDRESS          0x1B    // Qtouch device ID
#define QT_I2C_ADDRESS_W        (QT_I2C_ADDRESS << 1)     // Definicja adresu I2C tzw SLA+W
#define QT_I2C_ADDRESS_R        ((QT_I2C_ADDRESS <<1)|1)  // Definicja adresu I2C tzw SLA+R
#define QT_CONFIG_ADDR          32                          // Adres od ktorego mozemy uzyc struktury do konfiguracji do odczytu z I2C


uint8_t addr7w,addr7r;



void i2c_set_addr7(uint8_t addr)
{
	addr7w = addr << 1;
	addr7r = addr7w | 1;
}

int main(void)
{

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN|	// Enable clock: GPIOB
					RCC_AHB1ENR_GPIOCEN;	// Enable clock: GPIOC
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;		// Enable clock: I2C1



	__DSB();	// Data Synchronization Barrier


	/*
	 *  Enable AF for selected PINS
	 */
	GPIOC->MODER |= GPIO_MODER_MODER9_1 ;   // AF: PC9 => MCO2

#if defined P6_7
	GPIOB->MODER |= GPIO_MODER_MODER6_1 |   // AF: PB6 => I2C1_SCL
					GPIO_MODER_MODER7_1; 	// AF: PB7 => I2C1_SDA


	GPIOB->OTYPER |= GPIO_OTYPER_OT_6|
					 GPIO_OTYPER_OT_7;

	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6|
					  GPIO_OSPEEDER_OSPEEDR7;

	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_0|
					GPIO_PUPDR_PUPDR7_0;

	/*
	 * Alternate functions are configured in ARFL for PINS 0..7
	 * and in ARFH for PINS 8..15
	 * Based on DS we will select appropiate AF0..AF7
	 */
	GPIOB->AFR[0] |= ( 1 << 30 ) | ( 1 << 26); // P6/P7 => AF4



#endif

#if defined P8_9
	GPIOB->MODER |= GPIO_MODER_MODER8_1 |   // AF: PB8 => I2C1_SCL
					GPIO_MODER_MODER9_1; 	// AF: PB9 => I2C1_SDA


	GPIOB->OTYPER |= GPIO_OTYPER_OT_8|
					 GPIO_OTYPER_OT_9;

	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8|
					  GPIO_OSPEEDER_OSPEEDR9;

	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR8_0|
					GPIO_PUPDR_PUPDR9_0;
#endif


	/*
	 * FREQ: Set frequencey based on APB1 clock
	 * when using HSI@8MHz this is 2MHz
	 */
	I2C1->CR2 |= 0x02;

	/*
	 * Depending on the frequency of said prescaler must be installed in
	 * accordance with the selected data rate.
	 * We choose the maximum, for standard mode - 100 kHz:
	 *
	 *  2MHz / 100 = 10 kHz;
	 *
	 */
	I2C1->CCR &= ~I2C_CCR_CCR;
	I2C1->CCR |= 10;

	/*
	 * clock period is equal to (1 / 2 MHz = 500 ns), therefore the maximum rise time:
	 * 1000 ns / 500 ns = 2 + 1 (plus one - a small margin) = 3
	 *
	 */
	I2C1->TRISE = 3;


	/*
	 * Enable perifpherial at the END
	 */
	I2C1->CR1 = I2C_CR1_ACK|
				I2C_CR1_PE;     // PE : Peripherial enable





	//		TWI_start();                        // Start
	//	    TWI_write(QT_I2C_ADDRESS_W);        // SLA+W
	//	    TWI_write(registerAddress);         // Address
	//	    TWI_stop();                         // Stop
	I2C1->CR1 |= I2C_CR1_START;
	while (!(I2C1->SR1 & I2C_SR1_SB)) {}

	I2C1->DR = QT_I2C_ADDRESS_W;

	while (!(I2C1->SR1 & I2C_SR1_ADDR))
	{
		uint16_t reg1 = I2C1->SR1;
		uint16_t reg2 = I2C1->SR2;
	}

	uint16_t reg1 = I2C1->SR1;
	uint16_t reg2 = I2C1->SR2;

	while (I2C1->SR1 & (I2C_SR1_BTF)){}
	I2C1->DR = 0x00;

	while (!(I2C1->SR1 & (I2C_SR1_BTF)));

	I2C1->CR1 |= I2C_CR1_STOP;





	//	    TWI_start();                        // Start
	//	    TWI_write(QT_I2C_ADDRESS_R);        // SLA+R
	//	    *ptrresult = TWI_read(ACK);         // Przypisujemy odczytana wartosc
	//	    TWI_read(NACK);                     // NACK
	//	    TWI_stop();                         // STOP
	I2C1->CR1 |= I2C_CR1_START;
	while (!(I2C1->SR1 & I2C_SR1_SB)) {}

	I2C1->DR = QT_I2C_ADDRESS_R;

	while (!(I2C1->SR1 & I2C_SR1_ADDR))
	{
		uint16_t reg1 = I2C1->SR1;
		uint16_t reg2 = I2C1->SR2;
	}

	 reg1 = I2C1->SR1;
	 reg2 = I2C1->SR2;

	uint32_t value = I2C1->DR;

	I2C1->CR1 &= ~I2C_CR1_ACK;
	I2C1->CR1 |= I2C_CR1_STOP;



	while(1)
	{



	}
}


