/**
 * @file test_pi_controller.c
 * @author Danielou Mounsande (mounsandedaniel@gmail.com)
 * @brief Unit tests for the PI controller module using the Unity framework.
 * @version 1.0
 * @date 2026-03-13
 * 
 * @details This file contains unit tests to verify the behavior of the PI controller,
 * including proportional action, integral accumulation, and anti-windup logic.
 * 
 * @see https://danielou.netlify.app/
 */

#include "unity.h"
#include "pi_controller.h"

/**
 * @brief Setup function called before each test.
 * Resets the controller parameters and state.
 */
void setUp(void) {
    pi_controller_init(1.0f, 0.0f, -100.0f, 100.0f);
    pi_controller_reset();
}

/**
 * @brief Teardown function called after each test.
 */
void tearDown(void) {
    // Cleanup if necessary
}

/**
 * @brief Tests basic proportional-only control.
 */
void test_pi_proportional_only(void) {
    // Kp = 1.0, Ki = 0.0
    pi_controller_init(1.0f, 0.0f, -100.0f, 100.0f);
    
    // Setpoint = 10, Actual = 0 -> Error = 10. Output = 1.0 * 10 = 10
    float output = pi_controller_update(10.0f, 0.0f, 0.1f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 10.0f, output);
    
    // Setpoint = 10, Actual = 10 -> Error = 0. Output = 0
    output = pi_controller_update(10.0f, 10.0f, 0.1f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, output);
}

/**
 * @brief Tests integral accumulation.
 */
void test_pi_integral_action(void) {
    // Kp = 0.0, Ki = 1.0
    pi_controller_init(0.0f, 1.0f, -100.0f, 100.0f);
    
    // Step 1: Error = 10, dt = 1.0. Integral should become 10.
    // Note: Implementation computes output = p_term + ki * integral_term.
    // On first call, integral_term is 0, so output will be 0.
    // Then it updates integral_term to 10.
    float output = pi_controller_update(10.0f, 0.0f, 1.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, output);
    
    // Step 2: On second call, output should use the previous integral (10).
    output = pi_controller_update(10.0f, 0.0f, 1.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 10.0f, output);
}

/**
 * @brief Tests output clamping (saturation).
 */
void test_pi_output_clamping(void) {
    // Min = 0, Max = 50
    pi_controller_init(10.0f, 0.0f, 0.0f, 50.0f);
    
    // Error = 10 -> P-Term = 100. Should be clamped to 50.
    float output = pi_controller_update(10.0f, 0.0f, 0.1f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 50.0f, output);
    
    // Error = -10 -> P-Term = -100. Should be clamped to 0.
    output = pi_controller_update(0.0f, 10.0f, 0.1f);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, output);
}

/**
 * @brief Tests the anti-windup mechanism.
 */
void test_pi_anti_windup(void) {
    // Max output = 50
    pi_controller_init(1.0f, 1.0f, 0.0f, 50.0f);
    
    // Saturation state: Error is large, output is at max.
    // Integral should stop growing to prevent windup.
    for(int i = 0; i < 100; i++) {
        pi_controller_update(100.0f, 0.0f, 1.0f);
    }
    
    // If we now reverse the error, the controller should respond immediately
    // instead of waiting for a huge integral to "unwind".
    // Setpoint = 0, Actual = 10 -> Error = -10. Output should drop.
    float output = pi_controller_update(0.0f, 10.0f, 1.0f);
    TEST_ASSERT_LESS_THAN(50.0f, output);
}

/**
 * @brief Main entry point for the PI controller test suite.
 */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_pi_proportional_only);
    RUN_TEST(test_pi_integral_action);
    RUN_TEST(test_pi_output_clamping);
    RUN_TEST(test_pi_anti_windup);
    return UNITY_END();
}
