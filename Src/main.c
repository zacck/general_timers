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
#include <stdbool.h>
#include <stdio.h>




#define BLUE_LED_PORT GPIOD
#define BLUE_LED_PIN 15

static void SetSystemClockTo16Mhz(void);
static void ConfigureTim6(void);
static void ConfigureTim4(void);
static void ConfigureTim2(void);
static void ConfigurePA0(void);
static void setupMCO1(void);
static void PwmOnTim3(void);
static void PA6asPWM(void);
static void ConfigureSysTick(void);

/*RTC*/

static void setupRTC(void);
static void setupLSI(void);
static void initRTC(void);
static void loadDate(void);
static void getDateTime(void);


 /* END RTC*/


static volatile bool led_on = 0;
static volatile bool tick_led_on = 0;
static volatile bool tim_led_on = 0;
static volatile bool rev_led_on = 0;

void delay(void)
{
	for(uint32_t i = 0 ; i < 500000/2 ; i ++);
}

int main(void)
{
	SetSystemClockTo16Mhz();
	ConfigureTim6();
	ConfigureTim4();
	ConfigureTim2();
	ConfigureSysTick();
	PwmOnTim3();






	//Turn on Blue LED with pure CMSIS
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	GPIOD->MODER &= ~GPIO_MODER_MODER15_1;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_15;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_0;
	GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR15_1;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_0;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_1;

	//Configure Orange LED
	GPIOD->MODER |= GPIO_MODER_MODER13_0;
	GPIOD->MODER &= ~GPIO_MODER_MODER13_1;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_13;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13_0;
	GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR13_1;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR13_0;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR13_1;

	//Configure Green LED
	GPIOD->MODER |= GPIO_MODER_MODER12_0;
	GPIOD->MODER &= ~GPIO_MODER_MODER12_1;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_12;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR12_0;
	GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12_1;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR12_0;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR12_1;


	setupMCO1();
	setupRTC();



	printf("We have SWD\n");



    /* Loop forever*/
	while(1)
	{

	}
}

/* RTC STUFF */

static void setupRTC(void){
	//1.setup LS
	setupLSI();
	//2.Select RTC Clock & Init RTC
	initRTC();
	//3.Set Time & Date
	loadDate();
	//4.Get Time & Date
	getDateTime();
	//5.Set & Get Alarm
	//6.Deactivate Alarm

}

static void setupLSI(void) {
	/* Enable the power interface clock*/
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	// Enable access to the backup domain
	PWR->CR |= PWR_CR_DBP;

	// Wait for the backup domain write protection disable bit to take effect
	while ((PWR->CR & PWR_CR_DBP) == 0) {

	}

	// Perform a backup domain reset
	RCC->BDCR |= RCC_BDCR_BDRST;

	// Release the backup domain reset
	RCC->BDCR &= ~RCC_BDCR_BDRST;

	// Enable the LSI oscillator
	RCC->CSR |= RCC_CSR_LSION;

	// Wait until LSI is ready
	while ((RCC->CSR & RCC_CSR_LSIRDY) == 0) {

	}

	printf("LSI READY\n");

	//select LSI as RTC CLOCK
	RCC->BDCR |= RCC_BDCR_RTCSEL_1;


	printf("LSI is RTC CLK\n");



}

void initRTC(void) {
	// Enable RTC
	RCC->BDCR |= RCC_BDCR_RTCEN;

	//Unlock Write Protection after backup domain reset
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	//Enter init mode
	RTC->ISR |= RTC_ISR_INIT;

	//wait until initf is set
	while((RTC->ISR & RTC_ISR_INITF) == 0){

	}



	//Prescalers
	//S
	RTC->PRER |= 0xF9;
	//A
	RTC->PRER |= 0x7F0000;




	printf("RTC inited \n");


}

static void loadDate(void) {
	// Load Date
	RTC->DR = ((0x3 << RTC_DR_DU_Pos) | (0x1 << RTC_DR_DT_Pos) | // Day: 13
			(0x6 << RTC_DR_MU_Pos) | (0x0 << RTC_DR_MT_Pos) | // Month: June (6)
			(0x4 << RTC_DR_WDU_Pos) |                  // Weekday: Thursday (4)
			(0x4 << RTC_DR_YU_Pos) | (0x2 << RTC_DR_YT_Pos)); // Year: 24 (2024)

	// Load Time
	RTC->TR = ((0x3 << RTC_TR_SU_Pos) | (0x3 << RTC_TR_ST_Pos) | // Seconds: 33
			(0x6 << RTC_TR_MNU_Pos) | (0x4 << RTC_TR_MNT_Pos) | // Minutes: 29
			(0x2 << RTC_TR_HU_Pos) | (0x2 << RTC_TR_HT_Pos)); // Hours: 14 (2 PM)

	// Exit initialization mode
	RTC->ISR &= ~RTC_ISR_INIT;

	// Lock Write Protection
	RTC->WPR = 0xFF;


	printf("Date & Time loaded\n");

}

static void getDateTime(void){
	//Ensure Synchronization
    RTC->ISR &= ~RTC_ISR_RSF;

	while((RTC->ISR & RTC_ISR_RSF) == 0){

	}

	uint32_t date = RTC->DR;
	uint8_t day = ((date & RTC_DR_DT_Msk) >> RTC_DR_DT_Pos) * 10
			+ ((date & RTC_DR_DU_Msk) >> RTC_DR_DU_Pos);
	uint8_t month = ((date & RTC_DR_MT_Msk) >> RTC_DR_MT_Pos) * 10
			+ ((date & RTC_DR_MU_Msk) >> RTC_DR_MU_Pos);
	uint8_t year = ((date & RTC_DR_YT_Msk) >> RTC_DR_YT_Pos) * 10
			+ ((date & RTC_DR_YU_Msk) >> RTC_DR_YU_Pos);
	uint8_t weekday = (date & RTC_DR_WDU_Msk) >> RTC_DR_WDU_Pos;

	uint32_t time = RTC->TR;
	uint8_t hours = ((time & RTC_TR_HT_Msk) >> RTC_TR_HT_Pos) * 10
			+ ((time & RTC_TR_HU_Msk) >> RTC_TR_HU_Pos);
	uint8_t minutes = ((time & RTC_TR_MNT_Msk) >> RTC_TR_MNT_Pos) * 10
			+ ((time & RTC_TR_MNU_Msk) >> RTC_TR_MNU_Pos);
	uint8_t seconds = ((time & RTC_TR_ST_Msk) >> RTC_TR_ST_Pos) * 10
			+ ((time & RTC_TR_SU_Msk) >> RTC_TR_SU_Pos);


    printf("Date: %02d-%02d-20%02d, Weekday: %d\n", day, month, year, weekday);
    printf("Time: %02d:%02d:%02d\n", hours, minutes, seconds);


}



/* END RTC */

/*
 * Configure PA6 to run PWM out put
 */
static void PA6asPWM(void){
	//Enabe IOPA clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	//AF mode
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	//Max Speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	//AF2 mode
	//GPIOA->AFR[0] |= GPIO_AFRL_AFRL6_2;
	GPIOA->AFR[0] = 0x02000000;
}


/*
 * Run PWM on tim3 channel 1
 */
static void PwmOnTim3(void){

	//Configure PA6 as output
	PA6asPWM();
	//enable timer 3 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->CR1 &= ~TIM_CR1_CEN;

	//Enable ARPE
	TIM3->CR1 |= TIM_CR1_ARPE;

	//dont pre scale this clock so it runs at 16 MHZ like sys
	TIM3->PSC = 0;

	//set the arr for a 300khz period
	TIM3->ARR = 1599;

	//use the ccr reg for duty of
	TIM3->CCR1 = 799;

	//set the timer to pwm mode 1 on channel 1
	TIM3->CCMR1 |=  6U << TIM_CCMR1_OC1M_Pos;
	//Enable channel 1 preload
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

	//Enable compare mode
	TIM3->CCER |= TIM_CCER_CC1E;




	//load prescaler and enable update events
	TIM3->EGR |= TIM_EGR_UG;
	TIM3->CR1 |= TIM_CR1_CEN;

	//wait for update evebt
	while(!(TIM3->SR & TIM_SR_UIF));

	// Clear Interrupt Flag
	TIM3->SR &= ~(TIM_SR_UIF);

	//Reset the counter
	TIM3->CNT = 0;

}

/*
 * Configure  PA8 for MCO1 so we can use it for timer output
 */
static void ConfigurePA8(void){

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	GPIOA->MODER |= GPIO_MODER_MODER8_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;
	GPIOA->AFR[1]   &= ~GPIO_AFRH_AFRH0;

}

/*
 * Configure LSE and ouput it on a pin so we can measure it with input compare
 * */
static void setupMCO1(void){
	ConfigurePA8();
	// POINT Clock output GPIO (PA8 in this case)
	RCC->CFGR &= ~RCC_CFGR_MCO1_Msk;
}




/*
 * Configure timer 3 so that we can use it for some delays
 */
static void ConfigureTim6(void){
	//enable clock for timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;


	// set a prescaler so we get 1MHz
	//Required freq = CLK / (PSC + 1)
	TIM6->PSC = 15;

	/* (1 MHz / 10000) = 100Hz = 100ms */
	/* So, this will generate the 1ms delay */
	TIM6->ARR = 99999;

	//Enable timer to generate update events
	TIM6->EGR |= TIM_EGR_UG;

	/* Enable the Interrupt */
	TIM6->DIER |= TIM_DIER_UIE;

	/* Clear the Interrupt Status */
	TIM6->SR &= ~TIM_SR_UIF;

	/* Enable NVIC Interrupt for Timer 6 */
	NVIC_EnableIRQ(TIM6_DAC_IRQn);

	/* Finally enable TIM3 module */
	TIM6->CR1 = (1 << 0);
}

void TIM6_DAC_IRQHandler(void) {
	//check if UIF flag is set
	if (TIM6->SR & TIM_SR_UIF) {
		//flip our LED
		tim_led_on = !tim_led_on;

		if (tim_led_on) {
			//Enable compare mode
			//TIM3->CCER &= ~TIM_CCER_CC1E;
			GPIOD->BSRR |= GPIO_BSRR_BS_15;
		} else {
			//Enable compare mode
			//TIM3->CCER |= TIM_CCER_CC1E;
			GPIOD->BSRR |= GPIO_BSRR_BR_15;
		}
		// Clear Interrupt Flag
		TIM6->SR &= ~(TIM_SR_UIF);
	}

}

/*
 * Configure  PA0 for Timer 2 so we can use it for input compare
 */
static void ConfigurePA0(void){

	//Turn on Blue LED with pure CMSIS
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	GPIOA->MODER |= GPIO_MODER_MODER0_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0;
	GPIOA->AFR[0]   |= GPIO_AFRH_AFRH0_1;

}



/*
 * Configure Timer 2 so we can use it for input compare
 */
static void ConfigureTim2(void){
	//Start with a known state
	TIM2->CR1 &= ~TIM_CR1_CEN;

	//enable clock for timer 4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	ConfigurePA0();


	// set a prescaler so we get 2MHz
	//Required freq = CLK / (PSC + 1)
	TIM2->PSC = 7;

	/*Let timer count to max*/
	TIM2->ARR = 1999999;

	//Select channel 1 as Input
	TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;

	//Set Filter
	TIM2->CCMR1 |= TIM_CCMR1_IC1F_3;

	//Select rising edge by resetting CC1NP
	TIM2->CCER &= ~TIM_CCER_CC1NP;
	TIM2->CCER &= ~TIM_CCER_CC1P;

	//Enable Capture Into reg
	TIM2->CCER |= TIM_CCER_CC1E;



	/* Enable the Interrupt */
	TIM2->DIER |= TIM_DIER_UIE;

	/* Clear the Interrupt Status */
	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->SR &= ~TIM_SR_CC1IF;


	/* Enable NVIC Interrupt for Timer 4 */
	NVIC_EnableIRQ(TIM2_IRQn);



	//the next two lines to reset the timer
	TIM2->CR1 &= ~TIM_CR1_UDIS;

	//Enable timer to generate update events
	TIM2->EGR |= TIM_EGR_UG;


	/* Finally enable TIM4 module */
	TIM2->CR1 |= TIM_CR1_CEN;
}

uint32_t revs  =  0;


void TIM2_IRQHandler(void){
	//check if UIF flag is set
	if(TIM2->SR & TIM_SR_CC1IF)
	{
		//flip our LED
		rev_led_on = !rev_led_on;

		revs =  TIM2->CCR1;

		if(rev_led_on) {
			GPIOD->BSRR |= GPIO_BSRR_BS_12;
		} else {
			GPIOD->BSRR |= GPIO_BSRR_BR_12;
		}
		// Clear Interrupt Flag
		TIM2->SR &= ~(TIM_SR_CC1IF);

	}

}




/*
 * Configure Timer 4 so we can use it for interrupts
 */
static void ConfigureTim4(void){
	//enable clock for timer 4
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

	// set a prescaler so we get 1KHz
	//Required freq = CLK / (PSC + 1)
	TIM4->PSC = 15999;

	/* (1 KHz / 1000) = 1Hz = 1s */
	/* So, this will generate the 1s delay */
	TIM4->ARR = 999;

	//Enable timer to generate update events
	TIM4->EGR |= TIM_EGR_UG;

	/* Enable the Interrupt */
	TIM4->DIER |= TIM_DIER_UIE;

	/* Clear the Interrupt Status */
	TIM4->SR &= ~TIM_SR_UIF;

	/* Enable NVIC Interrupt for Timer 4 */
	NVIC_EnableIRQ(TIM4_IRQn);

	/* Finally enable TIM4 module */
	TIM4->CR1 |= TIM_CR1_CEN;
}

void TIM4_IRQHandler(void){
	//check if UIF flag is set
	if(TIM4->SR & TIM_SR_UIF)
	{
		//flip our LED
		led_on = !led_on;

		if (led_on) {
			getDateTime();
			GPIOD->BSRR |= GPIO_BSRR_BS_13;
		} else {
			GPIOD->BSRR |= GPIO_BSRR_BR_13;
		}
		// Clear Interrupt Flag
		TIM4->SR &= ~(TIM_SR_UIF);
	}

}

/*
 * Lets Enable Systick
 */
static void ConfigureSysTick(void){
	// We should compute this from the registers maybe?
	SysTick_Config(1600000 - 1);
}


void SysTick_Handler(void) {

	//flip our LED
	tick_led_on = !tick_led_on;

	if (tick_led_on) {
		GPIOD->BSRR |= GPIO_BSRR_BS_12;
	} else {
		GPIOD->BSRR |= GPIO_BSRR_BR_12;
	}

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






