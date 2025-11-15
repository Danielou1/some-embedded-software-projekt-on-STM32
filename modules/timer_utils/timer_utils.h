/**
 ******************************************************************************
 * @file    	timer_utils.h
 * @author		Danielou Mounsande
 * @version 	V2.0
 * @date		10.11.2025
 * @brief  	    Module for a non-blocking ms tick counter using a hardware timer.
 ******************************************************************************
 */

#ifndef TIMER_UTILS_TIMER_UTILS_H_
#define TIMER_UTILS_TIMER_UTILS_H_

 /* Includes */
#include "stm32f4xx.h"

/* Public variables */
extern TIM_HandleTypeDef htim6;

/* Public functions (prototypes) */

/**
 * @brief Initializes the hardware timer (TIM6) to generate a 1ms tick.
 */
void timer_utils_init(void);

/**
 * @brief  Gets the current value of the millisecond tick counter.
 * @return The number of milliseconds elapsed since timer_utils_init was called.
 */
uint32_t timer_utils_get_ticks(void);

/**
 * @brief  Handles the timer tick event.
 * @note   This function must be called from the HAL_TIM_PeriodElapsedCallback
 *         when the interrupt is from the timer used by this module (TIM6).
 */
void timer_utils_handle_tick(void);

#endif /* TIMER_UTILS_TIMER_UTILS_H_ */
