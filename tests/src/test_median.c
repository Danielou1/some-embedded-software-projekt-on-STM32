/**
 * @file test_median.c
 * @author Danielou Mounsande (mounsandedaniel@gmail.com)
 * @brief Unit tests for the median filter module using the Unity framework.
 * @version 1.0
 * @date 2026-03-13
 * 
 * @details This file contains unit tests to verify the behavior of the median filter,
 * including basic filtering of outliers and sequence processing.
 * 
 * @see https://danielou.netlify.app/
 */

#include "unity.h"
#include "median.h"

/**
 * @brief Setup function called before each test.
 */
void setUp(void) {
    // Initialization if necessary
}

/**
 * @brief Teardown function called after each test.
 */
void tearDown(void) {
    // Cleanup if necessary
}

/**
 * @brief Tests the basic filtering capability of the median filter.
 * 
 * Verifies that a single large outlier (noise) is correctly filtered out
 * and that the output remains within expected bounds.
 */
void test_median_basic_filtering(void) {
    // The filter has a length of 9 (MEDIAN_BUFFER_LENGTH)
    // Fill the buffer with stable values
    for(int i=0; i<8; i++) {
        median_get_median(10);
    }
    
    // Add a very high value (noise/outlier)
    uint32_t result = median_get_median(100);
    
    // The result should not be 100, as the 100 is filtered
    // In the implementation, there is also smoothing: (4*last + 1*current)/5
    // Therefore, the result should be close to 10
    TEST_ASSERT_LESS_THAN(50, result);
    TEST_ASSERT_GREATER_THAN(5, result);
}

/**
 * @brief Tests the response of the median filter to a sequence of values.
 * 
 * Verifies that the filter correctly handles a ramp of values and produces
 * a smoothed output.
 */
void test_median_sequence(void) {
    // Test if the filter responds correctly to a ramp
    median_get_median(10);
    median_get_median(11);
    median_get_median(12);
    median_get_median(13);
    uint32_t result = median_get_median(14); // 5th element
    
    // Since the buffer is not yet full of 14, 
    // and with smoothing, the value should increase gradually
    TEST_ASSERT_INT_WITHIN(5, 12, result);
}

/**
 * @brief Main entry point for the test suite.
 * @return int 0 if all tests passed, non-zero otherwise.
 */
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_median_basic_filtering);
    RUN_TEST(test_median_sequence);
    return UNITY_END();
}
