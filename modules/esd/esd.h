/**
  ******************************************************************************
  * @file    	esd.h
  * @author		Danielou Mounsande...
  * @version 	V1.0
  * @date		27.10.2022
  * @brief  	Module for the periphery 8-Segment LED BOARD
  ******************************************************************************
*/
#include "stm32f4xx.h"
#ifndef ESD_ESD_H_
#define ESD_ESD_H_

/* Inklusionen */
#include "stm32f4xx.h"

/* public Defines */
#define CNTL_PORT GPIOD
#define LED_A_F_PORT GPIOD
#define POINT_DOT_G_PORT GPIOE

#define CNTL1 GPIO_PIN_14
#define CNTL2 GPIO_PIN_15
#define CNTL3 GPIO_PIN_0
#define CNTL4 GPIO_PIN_1
#define LED_A GPIO_PIN_7
#define LED_B GPIO_PIN_4
#define LED_C GPIO_PIN_5
#define LED_D GPIO_PIN_6
#define LED_E GPIO_PIN_12
#define LED_F GPIO_PIN_11

#define LED_G GPIO_PIN_12 //PORT E
#define POINT GPIO_PIN_7 // PORT E
#define DOT GPIO_PIN_11 // PORT E

#define ALL_CNTL (CNTL1 | CNTL2 | CNTL3 | CNTL4)

#define DIGIT_0 (LED_G)
#define DIGIT_1 (LED_A | LED_D | LED_E | LED_F)
#define DIGIT_2 (LED_C | LED_F)
#define DIGIT_3 (LED_E | LED_F)
#define DIGIT_4 (LED_A | LED_D | LED_E)
#define DIGIT_5 (LED_B |LED_E)
#define DIGIT_6 (LED_B)
#define DIGIT_7 (LED_D | LED_E | LED_F)
#define DIGIT_9 (LED_E)

#define ALLE_LEDS_OHNE_G (LED_A | LED_B | LED_C | LED_D | LED_E | LED_F)

/* Enums */
typedef enum {
  ESD_DIGIT_0,
  ESD_DIGIT_1,
  ESD_DIGIT_2,
  ESD_DIGIT_3,
  ESD_DIGIT_4,
  ESD_DIGIT_5,
  ESD_DIGIT_6,
  ESD_DIGIT_7,
  ESD_DIGIT_8,
  ESD_DIGIT_9
} esd_digit_t;

typedef enum {
  ESD_POSITION_1,
  ESD_POSITION_2,
  ESD_POSITION_3,
  ESD_POSITION_4,
  ESD_POSITION_ALL
} esd_position_t;

/* Public functions (prototypes) */
void esd_init(void);
void esd_show_digit(esd_digit_t digit, esd_position_t pos);

#endif /* ESD_ESD_H_ */
