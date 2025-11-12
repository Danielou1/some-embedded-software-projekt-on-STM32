/**
 ******************************************************************************
 * @file    	pwm_dimmer.h
 * @author		Danielou Mounsande
 * @version 	V2.0
 * @date		12-November-2025
 * @brief		Header for a generic, frequency-based PWM dimmer module.
 *
 * @note    This module provides a hardware-independent way to control a GPIO
 *          pin's brightness or generate a specific frequency signal using PWM.
 *          It is configured via a struct, making it reusable.
 ******************************************************************************
 */

#ifndef MODULES_PWM_DIMMER_PWM_DIMMER_H_
#define MODULES_PWM_DIMMER_PWM_DIMMER_H_

#include "stm32f4xx.h"

/**
 * @brief Configuration structure for a PWM output channel.
 * @note  This struct holds all the necessary hardware-specific information
 *        to configure a PWM output on a given pin.
 */
typedef struct {
	TIM_TypeDef*  tim_instance; /*!< Pointer to the timer instance (e.g., TIM1). */
	uint32_t      tim_channel;  /*!< The timer channel to use (e.g., TIM_CHANNEL_2). */
	GPIO_TypeDef* gpio_port;    /*!< Pointer to the GPIO port for the output pin (e.g., GPIOE). */
	uint16_t      gpio_pin;     /*!< The GPIO pin number (e.g., GPIO_PIN_11). */
	uint8_t       gpio_af;      /*!< The alternate function mapping for the pin (e.g., GPIO_AF1_TIM1). */
} PWM_Dimmer_Config_t;


/**
 * @brief  Initializes a timer channel in PWM mode for a specific pin and frequency.
 * @note   This function calculates the optimal prescaler and period to achieve the
 *         desired frequency, then configures the hardware and starts the PWM signal.
 * @param  config: Pointer to a `PWM_Dimmer_Config_t` struct containing all hardware parameters.
 * @param  frequency: The desired PWM frequency in Hz.
 * @param  initial_duty_cycle: The initial duty cycle, from 0.0f (0%) to 1.0f (100%).
 * @retval None
 */
void pwm_dimmer_init(PWM_Dimmer_Config_t* config, uint32_t frequency, float initial_duty_cycle);

/**
 * @brief  Sets the duty cycle of the PWM signal.
 * @param  duty_cycle: The new duty cycle, from 0.0f (0%) to 1.0f (100%).
 * @retval None
 */
void pwm_dimmer_set_duty_cycle(float duty_cycle);


#endif /* MODULES_PWM_DIMMER_PWM_DIMMER_H_ */
