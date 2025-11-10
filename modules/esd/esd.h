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

/* Data types */
typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} ESD_Pin_t;

typedef struct {
	ESD_Pin_t cntl1;
	ESD_Pin_t cntl2;
	ESD_Pin_t cntl3;
	ESD_Pin_t cntl4;
	ESD_Pin_t led_a;
	ESD_Pin_t led_b;
	ESD_Pin_t led_c;
	ESD_Pin_t led_d;
	ESD_Pin_t led_e;
	ESD_Pin_t led_f;
	ESD_Pin_t led_g;
	ESD_Pin_t point;
	ESD_Pin_t dot;
} ESD_Config_t;

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
void esd_init(ESD_Config_t* config);
void esd_show_digit(esd_digit_t digit, esd_position_t pos);

#endif /* ESD_ESD_H_ */
