/**
 ******************************************************************************
 * @file    	stopwatch.h
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		12-November-2025
 * @brief		Header for a high-precision stopwatch/lap timer module.
 *
 * @note    This module uses an external interrupt (EXTI) on the user button
 *          for event capture (start/lap) and the `timer_utils` module for
 *          millisecond-precision timekeeping.
 ******************************************************************************
 */

#ifndef MODULES_STOPWATCH_STOPWATCH_H_
#define MODULES_STOPWATCH_STOPWATCH_H_

#include "stm32f4xx.h"
#include <stdint.h>

/* Public constants ----------------------------------------------------------*/
#define STOPWATCH_MAX_LAPS 10 // Maximum number of laps that can be stored

/* Public enums --------------------------------------------------------------*/

/**
 * @brief Defines the possible states of the stopwatch.
 */
typedef enum {
	STOPWATCH_STOPPED, /*!< The stopwatch is stopped and reset to zero. */
	STOPWATCH_RUNNING, /*!< The stopwatch is actively counting time. */
	STOPWATCH_PAUSED   /*!< The stopwatch is paused at a specific time. */
} Stopwatch_State_t;

/* Public function prototypes ------------------------------------------------*/

/**
 * @brief Initializes the stopwatch module.
 * @note  Configures the user button (PA0) with an external interrupt on the rising edge.
 *        This must be called once before using any other stopwatch function.
 */
void stopwatch_init(void);

/**
 * @brief Handles a button press event.
 * @note  This function should be called from the EXTI interrupt callback.
 *        It contains the core state machine for the stopwatch (Start/Lap/Pause).
 */
void stopwatch_handle_button_press(void);

/**
 * @brief Resets the stopwatch to its initial state.
 * @note  Clears all timings and lap data.
 */
void stopwatch_reset(void);

/**
 * @brief Pauses the stopwatch if it is currently running.
 */
void stopwatch_pause(void);

/**
 * @brief  Gets the current state of the stopwatch.
 * @return The current state (STOPWATCH_STOPPED, STOPWATCH_RUNNING, or STOPWATCH_PAUSED).
 */
Stopwatch_State_t stopwatch_get_state(void);

/**
 * @brief  Gets the total elapsed time.
 * @return Total elapsed time in milliseconds.
 */
uint32_t stopwatch_get_elapsed_ms(void);

/**
 * @brief  Gets the number of laps recorded.
 * @return The current number of recorded laps.
 */
uint8_t stopwatch_get_lap_count(void);

/**
 * @brief  Gets the time for a specific lap.
 * @param  lap_index: The index of the lap to retrieve (0-based).
 * @return The time for the specified lap in milliseconds. Returns 0 if index is invalid.
 */
uint32_t stopwatch_get_lap_ms(uint8_t lap_index);


#endif /* MODULES_STOPWATCH_STOPWATCH_H_ */
