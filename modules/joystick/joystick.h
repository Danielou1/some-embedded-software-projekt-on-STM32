/**
  ******************************************************************************
  * @file    	Joystick.h
  * @author		Danielou Mounsande
  * @version 	V1.0
  * @date		30.04.2021
  * @brief  	Module for using the on-board joystick of the waveshare-board.
  ******************************************************************************
*/

#ifndef JOYSTICK_JOYSTICK_H_
#define JOYSTICK_JOYSTICK_H_


#include "stm32f4xx.h"


/* --- Public Datatypes */

/* Struct for the joystick configuration */
typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin_right;
	uint16_t pin_down;
	uint16_t pin_left;
	uint16_t pin_up;
	uint16_t pin_select;
} Joystick_Config_t;


/* --- Public functions (prototypes) */

void joystick_init(Joystick_Config_t* config);
uint16_t joystick_read(void);


#endif /* JOYSTICK_JOYSTICK_H_ */
