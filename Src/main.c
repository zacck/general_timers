/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "main.h"

#define BLUE_LED_PORT GPIOD
#define BLUE_LED_PIN 15

static void SetSystemClockTo16Mhz(void);

int main(void)
{
	SetSystemClockTo16Mhz();

	//Turn on Blue LED with pure CMSIS
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	GPIOD->MODER &= ~GPIO_MODER_MODER15_1;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_15;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_0;
	GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR15_1;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_0;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_1;


	GPIOD->BSRR |= GPIO_BSRR_BS_15;


    /* Loop forever */
	for(;;);
}


/*
 * Function to set system clock to 16MHz
 */
static void SetSystemClockTo16Mhz(void) {

	// check if HSI is ready
	if((RCC->CR & RCC_CR_HSIRDY) == 0){
		//set HSI as clock
		RCC->CR |= RCC_CR_HSION;

		//wait until system clock swaps
		while((RCC->CR & RCC_CR_HSIRDY) == 0);

	}

	//set AHB prescaler to 1
	RCC->CFGR  |= RCC_CFGR_HPRE_DIV1;

	//APB1 prescaler
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;

	//APB2 Prescaler
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	//select HSI as system clock
	RCC->CFGR &= (uint32_t)(uint32_t)~(RCC_CFGR_SW);
	RCC->CFGR |= RCC_CFGR_SW_HSI;

	/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
	FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_3WS;


	/* Disabling HSE Clock*/
	RCC->CR &= ~RCC_CR_HSEON;



}






