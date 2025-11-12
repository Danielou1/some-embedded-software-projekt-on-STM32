/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V3.1
 * @date    12-November-2025
 * @brief   Main program for the 01_ESD project.
 *
 * @note    This project drives a 4-digit 7-segment display (ESD Board)
 *          to show a countdown from 9 to 0 on each position, one after
 *          the other. It uses a non-blocking timer for updates.
 *          This `main.c` serves as a usage example for the `esd` and `timer_utils` modules.
 *
 * @section WIRING Wiring Instructions
 * Connect the "8-SEG-LED-Board" to the microcontroller via the 2x10
 * pin connector (Header 10x2) as follows:
 *
 * | Display Pin Name    | Header Pin | STM32 Pin |
 * |:-------------------:|:----------:|:---------:|
 * | VCC3V3              | 1, 2       | +3.3V     |
 * | GND                 | 3, 4       | GND       |
 * | CNTL1 (Digit 1)     | 5          | PD14      |
 * | a (Segment a)       | 6          | PD7       |
 * | CNTL2 (Digit 2)     | 7          | PD15      |
 * | b (Segment b)       | 8          | PD4       |
 * | CNTL3 (Digit 3)     | 9          | PD0       |
 * | c (Segment c)       | 10         | PD5       |
 * | CNTL4 (Digit 4)     | 11         | PD1       |
 * | d (Segment d)       | 12         | PD6       |
 * | point (Decimal Pt.) | 13         | PE7       |
 * | e (Segment e)       | 14         | PD12      |
 * | f (Segment f)       | 16         | PD11      |
 * | g (Segment g)       | 17         | PE12      |
 * | dot (Colon)         | 19         | PE11      |
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "esd/esd.h"
#include "timer_utils/timer_utils.h"

/* Pin Configuration ---------------------------------------------------------*/

// Defines the mapping between the hardware and the esd module.
// This structure will be passed to the module so it knows which pins to drive.
ESD_Config_t esd_config = {
    .cntl1 = { .port = GPIOD, .pin = GPIO_PIN_14 },
    .cntl2 = { .port = GPIOD, .pin = GPIO_PIN_15 },
    .cntl3 = { .port = GPIOD, .pin = GPIO_PIN_0 },
    .cntl4 = { .port = GPIOD, .pin = GPIO_PIN_1 },
    .led_a = { .port = GPIOD, .pin = GPIO_PIN_7 },
    .led_b = { .port = GPIOD, .pin = GPIO_PIN_4 },
    .led_c = { .port = GPIOD, .pin = GPIO_PIN_5 },
    .led_d = { .port = GPIOD, .pin = GPIO_PIN_6 },
    .led_e = { .port = GPIOD, .pin = GPIO_PIN_12 },
    .led_f = { .port = GPIOD, .pin = GPIO_PIN_11 },
    .led_g = { .port = GPIOE, .pin = GPIO_PIN_12 },
    .point = { .port = GPIOE, .pin = GPIO_PIN_7 },
    .dot   = { .port = GPIOE, .pin = GPIO_PIN_11 }
};

/* Main Program --------------------------------------------------------------*/

/**
 * @brief  Application entry point.
 * @retval int
 */
int main(void) {
    /* Hardware and module initialization */
    HAL_Init();
    timer_utils_init();     // Initialize the non-blocking timer
    esd_init(&esd_config);  // Initialize the display module (now manages its own GPIOs)

    /* Data preparation for the display */
    esd_digit_t digitArray[10] = {
        ESD_DIGIT_0, ESD_DIGIT_1, ESD_DIGIT_2, ESD_DIGIT_3, ESD_DIGIT_4,
        ESD_DIGIT_5, ESD_DIGIT_6, ESD_DIGIT_7, ESD_DIGIT_8, ESD_DIGIT_9
    };
    esd_position_t positionArray[4] = {
        ESD_POSITION_1, ESD_POSITION_2, ESD_POSITION_3, ESD_POSITION_4
    };

    /* Application logic variables */
    int digitCounter = 9;
    int posCounter = 0;
    uint32_t last_update = 0;

    /* Infinite loop */
    while (1) {
        // Use a non-blocking timer to execute this code every 1000ms (1s)
        if (timer_utils_get_ticks() - last_update >= 1000) {
            // Display the current digit at the current position
            esd_show_digit(digitArray[digitCounter], positionArray[posCounter]);

            // Countdown logic
            digitCounter--;
            if (digitCounter < 0) {
                digitCounter = 9; // Reset the digit
                posCounter++;     // Move to the next position
                if (posCounter >= 4) {
                    posCounter = 0; // Wrap around to the first position
                }
            }

            // Update the time of the last display update
            last_update = timer_utils_get_ticks();
        }
        // The CPU is free here to do other tasks between display updates.
    }
}
