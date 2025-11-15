/**
 ******************************************************************************
 * @file    	tacho_reader.c
 * @author		Danielou Mounsande
 * @version 	V1.1
 * @date		reviewed on 12-November-2025
 * @brief		Implementation for a fan tachometer reader module.
 ******************************************************************************
 */

#include "tacho_reader/tacho_reader.h"
#include "median/median.h"

/* Private definitions -------------------------------------------------------*/
#define TACHO_GPIO_PORT GPIOC
#define TACHO_GPIO_PIN  GPIO_PIN_7
#define TACHO_IRQn      EXTI9_5_IRQn

/* Private variables ---------------------------------------------------------*/
static TIM_HandleTypeDef g_tim_handle;              // Handle for the measurement timer
static volatile uint32_t g_last_capture = 0;        // Timer value at the last pulse
static volatile uint32_t g_filtered_rpm = 0;        // Stores the last known filtered RPM

/* Public function implementations -------------------------------------------*/

/**
 * @brief Initializes the tachometer reader module.
 */
void tacho_reader_init(void)
{
    // --- 1. GPIO Init (PC7) ---
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TACHO_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Interrupt on falling edge as the fan pulls the line low
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // Use internal pull-up as required
    HAL_GPIO_Init(TACHO_GPIO_PORT, &GPIO_InitStruct);

    // --- 2. EXTI Init ---
    HAL_NVIC_SetPriority(TACHO_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TACHO_IRQn);

    // --- 3. Timer Init (TIM2 as 1MHz time base) ---
    __HAL_RCC_TIM2_CLK_ENABLE();
    // APB1 Timer Clock is 42MHz on this board. Prescaler of 41 gives 42MHz / (41+1) = 1MHz clock.
    g_tim_handle.Instance = TIM2;
    g_tim_handle.Init.Prescaler = 83;
    g_tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim_handle.Init.Period = 0xFFFFFFFF; // Max period for 32-bit timer
    g_tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&g_tim_handle);
    HAL_TIM_Base_Start(&g_tim_handle);
}

/**
 * @brief Handles a tacho pulse event.
 */
void tacho_reader_handle_pulse(void)
{
    uint32_t current_capture = __HAL_TIM_GET_COUNTER(&g_tim_handle);
    uint32_t pulse_interval = 0;
    uint32_t calculated_rpm = 0;

    // Calculate interval, handling timer overflow
    if (current_capture > g_last_capture) {
        pulse_interval = current_capture - g_last_capture;
    } else {
        // Timer has overflowed
        pulse_interval = (0xFFFFFFFF - g_last_capture) + current_capture;
    }
    g_last_capture = current_capture;

    // Avoid division by zero if fan is stopped or too slow
    if (pulse_interval > 0) {
        // Formula: RPM = 60 / (2 * t_seconds) = 30 / t_seconds
        // t_seconds = pulse_interval_us / 1,000,000
        // RPM = 30 * 1,000,000 / pulse_interval_us
        calculated_rpm = 30000000 / pulse_interval;
    }

    // Update the median filter with the new raw RPM value and store the result
    g_filtered_rpm = median_get_median(calculated_rpm);
}

/**
 * @brief  Gets the last calculated fan speed, filtered.
 */
uint32_t tacho_reader_get_rpm(void)
{
    // Check if the fan has stopped (if the last pulse was too long ago)
    uint32_t time_since_last_pulse = __HAL_TIM_GET_COUNTER(&g_tim_handle) - g_last_capture;
    // If more than ~0.5 second has passed, assume fan is stopped.
    if (time_since_last_pulse > 500000) {
        // Feed zeros to the filter to clear it out over time
        g_filtered_rpm = median_get_median(0);
        return 0;
    }

    return g_filtered_rpm;
}
