/**
 ******************************************************************************
 * @file    	pi_controller.h
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		15-November-2025
 * @brief		Header for a simple Proportional-Integral (PI) controller.
 ******************************************************************************
 */

#ifndef MODULES_PI_CONTROLLER_PI_CONTROLLER_H_
#define MODULES_PI_CONTROLLER_PI_CONTROLLER_H_

#include <stdint.h>

/**
 * @brief Initializes the PI controller with its parameters.
 * @param kp The proportional gain.
 * @param ki The integral gain.
 * @param output_min The minimum value for the controller's output.
 * @param output_max The maximum value for the controller's output.
 */
void pi_controller_init(float kp, float ki, float output_min, float output_max);

/**
 * @brief Updates the controller with new values and computes the new output.
 * @param setpoint The desired value (e.g., target RPM).
 * @param actual_value The measured value (e.g., current RPM).
 * @param dt The time delta in seconds since the last update (Ta).
 * @return The computed control output (e.g., PWM duty cycle).
 */
float pi_controller_update(float setpoint, float actual_value, float dt);

/**
 * @brief Resets the internal state of the controller.
 * @note  This is important to prevent integral windup when the controller is disabled.
 */
void pi_controller_reset(void);


#endif /* MODULES_PI_CONTROLLER_PI_CONTROLLER_H_ */
