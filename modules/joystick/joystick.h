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


/* --- Public Preprocessor defines */

/* Mapping of joystick gpio-port and -pins. */
#define	JOYSTICK_PORT		GPIOG
#define JOYSTICK_PIN_RIGHT 	GPIO_PIN_10
#define JOYSTICK_PIN_DOWN	GPIO_PIN_6
#define JOYSTICK_PIN_LEFT 	GPIO_PIN_9
#define JOYSTICK_PIN_UP 	GPIO_PIN_11
#define JOYSTICK_PIN_SELECT GPIO_PIN_12
#define JOYSTICK_PIN_ALL	(JOYSTICK_PIN_RIGHT | JOYSTICK_PIN_DOWN | JOYSTICK_PIN_LEFT | JOYSTICK_PIN_UP | JOYSTICK_PIN_SELECT )


/* --- Public functions (prototypes) */

void joystick_init(void);
uint16_t joystick_read(void);


#endif /* JOYSTICK_JOYSTICK_H_ */
