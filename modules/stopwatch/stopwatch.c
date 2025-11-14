/**
 ******************************************************************************
 * @file    	stopwatch.c
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		12-November-2025
 * @brief		Implementation for a high-precision stopwatch/lap timer module.
 ******************************************************************************
 */

#include "stopwatch/stopwatch.h"
#include "timer_utils/timer_utils.h"
#include <string.h> // For memset

/* Private data structures ---------------------------------------------------*/

/**
 * @brief Holds all the internal data for the stopwatch module.
 */
static struct {
	volatile Stopwatch_State_t state;         /*!< Current state of the stopwatch. Volatile as it's modified in an ISR. */
	volatile uint32_t start_time_ms;      /*!< Timestamp when the stopwatch was started. */
	volatile uint32_t paused_time_ms;     /*!< Total time elapsed when the stopwatch was paused. */
	volatile uint32_t last_lap_time_ms;   /*!< Timestamp of the last lap event, used for calculating lap duration. */
	uint32_t laps[STOPWATCH_MAX_LAPS];        /*!< Array to store recorded lap times. */
	volatile uint8_t lap_count;           /*!< Number of laps currently recorded. */
} g_stopwatch;


/* Public function implementations -------------------------------------------*/

void stopwatch_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. Enable GPIOA clock for the user button
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 2. Configure PA0 as input with interrupt on rising edge
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. Enable and set EXTI Line0 Interrupt priority
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    // 4. Set the stopwatch to its initial state
    stopwatch_reset();
}

void stopwatch_handle_button_press(void)
{
	uint32_t current_time = timer_utils_get_ticks();

	switch (g_stopwatch.state)
	{
		case STOPWATCH_STOPPED:
			// First press starts the stopwatch
			g_stopwatch.start_time_ms = current_time;
			g_stopwatch.last_lap_time_ms = current_time;
			g_stopwatch.lap_count = 0;
			g_stopwatch.paused_time_ms = 0;
			memset(g_stopwatch.laps, 0, sizeof(g_stopwatch.laps));
			g_stopwatch.state = STOPWATCH_RUNNING;
			break;

		case STOPWATCH_RUNNING:
			// Subsequent presses record a lap
			if (g_stopwatch.lap_count < STOPWATCH_MAX_LAPS)
			{
				g_stopwatch.laps[g_stopwatch.lap_count] = current_time - g_stopwatch.last_lap_time_ms;
				g_stopwatch.last_lap_time_ms = current_time;
				g_stopwatch.lap_count++;
			}
			break;

		case STOPWATCH_PAUSED:
			// Pressing while paused resumes the stopwatch
			// We adjust the start time to account for the paused duration
			g_stopwatch.start_time_ms += (current_time - g_stopwatch.paused_time_ms);
			g_stopwatch.last_lap_time_ms += (current_time - g_stopwatch.paused_time_ms);
			g_stopwatch.state = STOPWATCH_RUNNING;
			break;
	}
}

void stopwatch_reset(void)
{
	g_stopwatch.state = STOPWATCH_STOPPED;
	g_stopwatch.start_time_ms = 0;
	g_stopwatch.paused_time_ms = 0;
	g_stopwatch.last_lap_time_ms = 0;
	g_stopwatch.lap_count = 0;
	memset(g_stopwatch.laps, 0, sizeof(g_stopwatch.laps));
}

// Note: This function is not in the header, but useful for the main loop logic
void stopwatch_pause(void)
{
	if (g_stopwatch.state == STOPWATCH_RUNNING)
	{
		g_stopwatch.paused_time_ms = timer_utils_get_ticks();
		g_stopwatch.state = STOPWATCH_PAUSED;
	}
}

Stopwatch_State_t stopwatch_get_state(void)
{
	return g_stopwatch.state;
}

uint32_t stopwatch_get_elapsed_ms(void)
{
	if (g_stopwatch.state == STOPWATCH_RUNNING)
	{
		return timer_utils_get_ticks() - g_stopwatch.start_time_ms;
	}
	else if (g_stopwatch.state == STOPWATCH_PAUSED)
	{
		return g_stopwatch.paused_time_ms - g_stopwatch.start_time_ms;
	}
	return 0;
}

uint8_t stopwatch_get_lap_count(void)
{
	return g_stopwatch.lap_count;
}

uint32_t stopwatch_get_lap_ms(uint8_t lap_index)
{
	if (lap_index < g_stopwatch.lap_count)
	{
		return g_stopwatch.laps[lap_index];
	}
	return 0;
}
