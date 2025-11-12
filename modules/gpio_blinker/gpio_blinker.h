/**
 ******************************************************************************
 * @file    	gpio_blinker.h
 * @author		Danielou Mounsande
 * @version 	V1.1
 * @date		12-November-2025
 * @brief		Header for a generic, non-blocking GPIO blinker module.
 *
 * @note    This module provides a simple way to make any GPIO pin blink
 *          at a specified interval without using blocking delays. It relies
 *          on the `timer_utils` module for timekeeping.
 ******************************************************************************
 */

#ifndef MODULES_GPIO_BLINKER_GPIO_BLINKER_H_
#define MODULES_GPIO_BLINKER_GPIO_BLINKER_H_

#include "stm32f4xx.h"

/**
 * @brief  Initializes the blinker module for a specific GPIO pin.
 * @note   This function configures the given pin as a push-pull output
 *         and enables its corresponding GPIO port clock. It should be
 *         called once before using the blinker.
 * @param  port: A pointer to the GPIO port (e.g., GPIOE).
 * @param  pin: The GPIO pin identifier (e.g., GPIO_PIN_11).
 * @retval None
 */
void gpio_blinker_init(GPIO_TypeDef* port, uint16_t pin);

/**
 * @brief  Updates the blinker state.
 * @note   This function must be called repeatedly in the main application loop
 *         to produce the blinking effect. It is non-blocking.
 * @param  interval_ms: The desired blinking interval in milliseconds.
 *         For example, 500ms will result in a 1Hz blink frequency (500ms ON, 500ms OFF).
 * @retval None
 */
void gpio_blinker_update(uint32_t interval_ms);


#endif /* MODULES_GPIO_BLINKER_GPIO_BLINKER_H_ */
