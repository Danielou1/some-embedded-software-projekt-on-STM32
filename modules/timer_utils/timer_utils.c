/**
 ******************************************************************************
 * @file    	timer_utils.c
 * @author		Danielou Mounsande
 * @version 	V2.0
 * @date		10.11.2025
 * @brief  	    Module for a non-blocking ms tick counter using TIM6.
 ******************************************************************************
 */

#include "timer_utils/timer_utils.h"

/* --- Global variables --- */
TIM_HandleTypeDef htim6;
static volatile uint32_t g_ticks = 0;


/* --- Public functions --- */

/**
 * @brief Initializes the hardware timer (TIM6) to generate a 1ms tick.
 */
void timer_utils_init(void) {
    // Enable TIM6 clock
    __HAL_RCC_TIM6_CLK_ENABLE();

    // Configure TIM6 for a 1ms update event
    // Assuming APB1 Timer Clock is 90MHz (for STM32F429 at 180MHz HCLK)
    // Prescaler = 90, so TIM6 clock is 90MHz / 90 = 1MHz (1us period)
    // Period (ARR) = 1000, so update event is every 1000 * 1us = 1ms
    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 90 - 1;
    htim6.Init.Period = 1000 - 1;
    HAL_TIM_Base_Init(&htim6);

    // Enable the TIM6 interrupt in the NVIC
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

    // Start the timer in interrupt mode
    HAL_TIM_Base_Start_IT(&htim6);
}

/**
 * @brief  Gets the current value of the millisecond tick counter.
 * @return The number of milliseconds elapsed since timer_utils_init was called.
 */
uint32_t timer_utils_get_ticks(void) {
    return g_ticks;
}


/* --- Interrupt Handlers and Callbacks --- */

/**
  * @brief  Period elapsed callback in non-blocking mode.
  * @note   This function is called automatically by the HAL library's ISR handler.
  * @param  htim: TIM handle
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    g_ticks++;
  }
}
