/**
 ******************************************************************************
 * @file    	pi_controller.c
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		15-November-2025
 * @brief		Implementation of a simple Proportional-Integral (PI) controller.
 ******************************************************************************
 */

#include "pi_controller/pi_controller.h"

// Private variables for the controller's state and parameters
static float g_kp = 0.0f;
static float g_ki = 0.0f;
static float g_output_min = 0.0f;
static float g_output_max = 1.0f;
static float g_integral_term = 0.0f;


/**
 * @brief Initializes the PI controller.
 */
void pi_controller_init(float kp, float ki, float output_min, float output_max)
{
    g_kp = kp;
    g_ki = ki;
    g_output_min = output_min;
    g_output_max = output_max;
    g_integral_term = 0.0f;
}

/**
 * @brief Updates the controller and computes the new output.
 */
float pi_controller_update(float setpoint, float actual_value, float dt)
{
    // 1. Calculate the error
    float error = setpoint - actual_value;

    // 2. Calculate the proportional term
    float p_term = g_kp * error;

    // 3. Calculate the potential integral term
    float i_term = g_ki * g_integral_term;

    // 4. Compute the total output
    float output = p_term + i_term;

    // 5. Clamp the output to its limits
    if (output > g_output_max) {
        output = g_output_max;
    } else if (output < g_output_min) {
        output = g_output_min;
    }

    // 6. Robust Anti-Windup (Conditional Integration):
    // Only integrate if the output is NOT saturated, OR if the error is
    // trying to bring the output away from the saturation point.
    // For example, if output is maxed out, we only integrate if error is negative.
    if ((error > 0 && output < g_output_max) || (error < 0 && output > g_output_min))
    {
        g_integral_term += error * dt;
    }

    return output;
}

/**
 * @brief Resets the internal state of the controller.
 */
void pi_controller_reset(void)
{
    g_integral_term = 0.0f;
}
