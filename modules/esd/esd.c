/**
  ******************************************************************************
  * @file    	esd.c
  * @author		Danielou Mounsande
  * @version 	V1.0
  * @date       18.04.2024
  ******************************************************************************
*/
#include "stm32f4xx.h"
/* Includes */
#include "esd/esd.h"

void esd_init(void){

	__HAL_RCC_GPIOD_CLK_ENABLE(); //Taktversorgung Port D
	__HAL_RCC_GPIOE_CLK_ENABLE(); //Taktversorgung Port E

	GPIO_InitTypeDef gpio_init; // eigene und separate Struktur für PORT E und G

		gpio_init.Pin = ALL_CNTL | ALLE_LEDS_OHNE_G; // Alle PIN auf PORT G
		gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
		gpio_init.Pull = GPIO_NOPULL;
		gpio_init.Speed = GPIO_SPEED_MEDIUM;

		HAL_GPIO_Init(GPIOD, &gpio_init);

		gpio_init.Pin = LED_G | POINT | DOT; //Alle PIN auf PORT E
		gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
		gpio_init.Pull = GPIO_NOPULL;
		gpio_init.Speed = GPIO_SPEED_MEDIUM;

		HAL_GPIO_Init(GPIOE, &gpio_init);



}
void esd_show_digit(esd_digit_t digit, esd_position_t pos) {

	HAL_GPIO_WritePin(GPIOD, ALL_CNTL, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, ALLE_LEDS_OHNE_G, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, LED_G, GPIO_PIN_RESET);

	switch (pos) {

	case ESD_POSITION_1:
		HAL_GPIO_WritePin(GPIOD, CNTL1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, DOT | POINT, GPIO_PIN_SET);
		break;
	case ESD_POSITION_2:
		HAL_GPIO_WritePin(GPIOD, CNTL2, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, DOT | POINT, GPIO_PIN_SET);
		break;
	case ESD_POSITION_3:
		HAL_GPIO_WritePin(GPIOD, CNTL3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, DOT | POINT, GPIO_PIN_SET);
		break;
	case ESD_POSITION_4:
		HAL_GPIO_WritePin(GPIOD, CNTL4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, DOT | POINT, GPIO_PIN_SET);
		break;
	case ESD_POSITION_ALL:
		HAL_GPIO_WritePin(GPIOD, CNTL1 | CNTL2 | CNTL3 | CNTL4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, DOT | POINT, GPIO_PIN_SET);
		break;
	}

	switch (digit) {

	case ESD_DIGIT_0:
		HAL_GPIO_WritePin(GPIOE, DIGIT_0, GPIO_PIN_SET);
		break;
	case ESD_DIGIT_1:
		HAL_GPIO_WritePin(GPIOD, DIGIT_1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, LED_G, GPIO_PIN_SET);
		break;
	case ESD_DIGIT_2:
		HAL_GPIO_WritePin(GPIOD, DIGIT_2, GPIO_PIN_SET);
		break;
	case ESD_DIGIT_3:
		HAL_GPIO_WritePin(GPIOD, DIGIT_3, GPIO_PIN_SET);
		break;
	case ESD_DIGIT_4:
		HAL_GPIO_WritePin(GPIOD, DIGIT_4, GPIO_PIN_SET);
		break;
	case ESD_DIGIT_5:
		HAL_GPIO_WritePin(GPIOD, DIGIT_5, GPIO_PIN_SET);
		break;
	case ESD_DIGIT_6:
		HAL_GPIO_WritePin(GPIOD, DIGIT_6, GPIO_PIN_SET);
		break;
	case ESD_DIGIT_7:
		HAL_GPIO_WritePin(GPIOD, DIGIT_7, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOE, LED_G, GPIO_PIN_SET);
		break;
	case ESD_DIGIT_8:
		break;
	case ESD_DIGIT_9:
		HAL_GPIO_WritePin(GPIOD, DIGIT_9, GPIO_PIN_SET);
		break;
	}

}
