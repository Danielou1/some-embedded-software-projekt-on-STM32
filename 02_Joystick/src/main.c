/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V3.1
 * @date    12-November-2025
 * @brief   Main program for the 02_Joystick project.
 *
 * @note    This project combines the use of a 7-segment display (ESD Board)
 *          and a digital joystick. The user can increment/decrement a digit,
 *          change its display position, and start a countdown using the joystick.
 *          The logic is non-blocking thanks to the `timer_utils` module.
 *
 * @section WIRING Wiring Instructions
 *
 * **1. 7-Segment Display (ESD Board):**
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
 * **2. Digital Joystick:**
 * The joystick is connected to the GPIOG port of the microcontroller.
 *
 * | Joystick Direction | STM32 Pin |
 * |:------------------:|:---------:|
 * | Right              | PG10      |
 * | Down               | PG6       |
 * | Left               | PG9       |
 * | Up                 | PG11      |
 * | Select (Center)    | PG12      |
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "esd/esd.h"
#include "joystick/joystick.h"
#include "timer_utils/timer_utils.h"

/* Pin Configurations --------------------------------------------------------*/

// Configuration for the 7-segment display
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

// Configuration for the joystick
Joystick_Config_t joystick_config = {
	.port = GPIOG,
	.pin_right = GPIO_PIN_10,
	.pin_down = GPIO_PIN_6,
	.pin_left = GPIO_PIN_9,
	.pin_up = GPIO_PIN_11,
	.pin_select = GPIO_PIN_12
};

/* Main Program --------------------------------------------------------------*/

/**
 * @brief  Application entry point.
 * @retval int
 */
int main(void) {
	/* Hardware and module initialization */
	HAL_Init();
	timer_utils_init();             // Initialize the non-blocking timer
	esd_init(&esd_config);          // Initialize the display module (manages its own GPIOs)
	joystick_init(&joystick_config);// Initialize the joystick module (manages its own GPIOs)

	/* Application variables */
	esd_position_t pos = ESD_POSITION_1; // Current display position
	esd_digit_t digit = ESD_DIGIT_0;     // Current digit to display
	uint8_t countdown_active = 0;        // Countdown active flag (0=false, 1=true)

	/* Time-keeping variables for non-blocking logic */
	uint32_t last_joystick_read = 0;
	uint32_t last_countdown_tick = 0;

	/* Infinite loop */
	while (1) {
		uint32_t current_ticks = timer_utils_get_ticks(); // Current time

		// --- Handle Countdown Logic (every 1000ms if active) ---
		if (countdown_active && (current_ticks - last_countdown_tick >= 1000)) {
			if (digit > ESD_DIGIT_0) {
				digit--; // Decrement the digit
			} else {
				countdown_active = 0; // Countdown finished
			}
			last_countdown_tick = current_ticks; // Update the time of the last tick
		}

		// --- Handle Joystick Input (every 200ms for polling/debouncing) ---
		// The joystick is only read if the countdown is not active
		if (!countdown_active && (current_ticks - last_joystick_read >= 200)) {
			uint16_t state = joystick_read(); // Read the joystick state

			// Increment digit (Up)
			if (state & joystick_config.pin_up) {
				digit = (digit + 1) % 10;
				last_joystick_read = current_ticks;
			}
			// Decrement digit (Down)
			else if (state & joystick_config.pin_down) {
				digit = (digit + 9) % 10; // (digit - 1 + 10) % 10 to handle wrapping from 0 to 9
				last_joystick_read = current_ticks;
			}
			// Change display position (Left)
			else if (state & joystick_config.pin_left) {
				pos = (pos + 3) % 4; // Cycle through positions 0-3
				last_joystick_read = current_ticks;
			}
			// Change display position (Right)
			else if (state & joystick_config.pin_right) {
				pos = (pos + 1) % 4; // Cycle through positions 0-3
				last_joystick_read = current_ticks;
			}
			// Start countdown (Select)
			else if (state & joystick_config.pin_select) {
				countdown_active = 1;
				last_countdown_tick = current_ticks; // Initialize the countdown timer
				last_joystick_read = current_ticks;  // Reset the joystick read timer
			}
		}

		// --- Handle Display Update (continuously for multiplexing) ---
		// This function must be called very frequently to give the illusion
		// that all digits are lit simultaneously.
		esd_show_digit(digit, pos);
	}
}
