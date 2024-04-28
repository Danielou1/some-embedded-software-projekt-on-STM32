/**
 ******************************************************************************
 * @file    	utils.h
 * 	@author		Danielou Mounsande
 * @version 	V1.0
 * @date		02.11.2022
 * @brief  	Module for using the periphery GPIO
 ******************************************************************************
 */

/* Public Preprocessor defines */
#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

 /* Includes */
#include "stm32f4xx.h"

/* Public functions (prototypes) */
void utils_delay_ms(uint32_t t);
void utils_gpio_port_write(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN);
uint16_t utils_gpio_port_read(GPIO_TypeDef *GPIOx);

#endif /* UTILS_UTILS_H_ */

