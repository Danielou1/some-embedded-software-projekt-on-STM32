/**
 ******************************************************************************
 * @file    	tacho_reader.h
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		12-November-2025
 * @brief		Header for a fan tachometer reader module.
 *
 * @note    This module measures the RPM of a fan by capturing pulses from
 *          its tachometer output on an external interrupt pin (PC7). It uses
 *          a dedicated timer to measure the time between pulses.
 ******************************************************************************
 */

#ifndef MODULES_TACHO_READER_TACHO_READER_H_
#define MODULES_TACHO_READER_TACHO_READER_H_

#include "stm32f4xx.h"
#include <stdint.h>

/**
 * @brief Initializes the tachometer reader module.
 * @note  Configures GPIO PC7 with an EXTI, and a timer for time measurement.
 */
void tacho_reader_init(void);

/**
 * @brief Handles a tacho pulse event.
 * @note  This function should be called from the EXTI interrupt callback
 *        for the tacho pin. It calculates the time since the last pulse.
 */
void tacho_reader_handle_pulse(void);

/**
 * @brief  Gets the last calculated fan speed, filtered by a median filter.
 * @return The fan speed in Revolutions Per Minute (RPM).
 */
uint32_t tacho_reader_get_rpm(void);


#endif /* MODULES_TACHO_READER_TACHO_READER_H_ */
