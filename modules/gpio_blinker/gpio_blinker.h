/**
 ******************************************************************************
 * @file    	gpio_blinker.h
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		10.11.2025
 * @brief		A simple non-blocking GPIO blinker module.
 ******************************************************************************
 */

#ifndef MODULES_GPIO_BLINKER_GPIO_BLINKER_H_
#define MODULES_GPIO_BLINKER_GPIO_BLINKER_H_

#include "stm32f4xx.h"

/**
 * @brief Initializes the blinker with a specific GPIO pin.
 * @param port The GPIO port (e.g., GPIOE).
 * @param pin The GPIO pin (e.g., GPIO_PIN_11).
 */
void gpio_blinker_init(GPIO_TypeDef* port, uint16_t pin);

/**
 * @brief Updates the blinker state. This function should be called
 *        repeatedly in the main loop.
 * @param interval_ms The blinking interval in milliseconds (e.g., 500 for a 1Hz blink).
 */
void gpio_blinker_update(uint32_t interval_ms);


#endif /* MODULES_GPIO_BLINKER_GPIO_BLINKER_H_ */
