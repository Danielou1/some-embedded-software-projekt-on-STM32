/**
 **********************************************************************************************************
 * @file    	delay_utils.c
 * 	@author		Mounsande Danielou & Danielle Ndjensi
 * @version 	V1.0
 * @date
 * @brief Module for using the periphery GPIO
@verbatim
 **********************************************************************************************************
 ==================================================
  ### Resources used ###
  GPIO
  ODR
  IDR
==================================================
  ### Usage ###
  (#) Call 'utils_delay_ms()' to delay the program execution for a certain amount of milliseconds
  (#) Call 'utils_gpio_port_write()' to write a value to the specified GPIO port
  (#) Call 'utils_gpio_port_read()' to read the current value of the specified GPIO port
==================================================
@endverbatim
**************************************************
 */

#include "delay_utils/delay_utils.h"

/**
* @brief Delays the program execution for a certain amount of milliseconds
* @param t: The amount of time to delay in milliseconds
* @return None
*/
void utils_delay_ms(uint32_t t) {
	HAL_Delay(t);
}

/**
* @brief Writes a value to the specified GPIO port
* @param GPIOx: The GPIO port to write to
* @param GPIO_PIN: The value to write to the port
* @return None
*/
void utils_gpio_port_write(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN) {

	GPIOx->ODR = 0 | GPIO_PIN; // reset the  ODR and Initialize
}

/**
* @brief Reads the current value of the specified GPIO port
* @param GPIOx: The GPIO port to read from
* @return The current value of the port
*/
uint16_t utils_gpio_port_read(GPIO_TypeDef *GPIOx) {

	return GPIOx->IDR;
}

