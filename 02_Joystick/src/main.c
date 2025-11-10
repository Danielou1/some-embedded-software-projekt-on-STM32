/**
 ******************************************************************************
 * @file    	main.c
 * @author      Danielou Mounsande & Danielle Ndjensi
 * @version 	V2.0
 * @date        10.11.2025
 * @brief		Vorlage mit dem 8BIT SEG und dem Joystick (non-blocking)
 ******************************************************************************
 */

/* Includes */
#include "stm32f4xx.h"
#include "esd/esd.h"
#include "joystick/joystick.h"
#include "timer_utils/timer_utils.h"

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

/**
 * @brief Initializes the GPIO pins for the 7-segment display.
 */
void ESD_GPIO_Init(void) {
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_MEDIUM;

    gpio_init.Pin = esd_config.cntl1.pin | esd_config.cntl2.pin | esd_config.cntl3.pin | esd_config.cntl4.pin |
                    esd_config.led_a.pin | esd_config.led_b.pin | esd_config.led_c.pin | esd_config.led_d.pin |
                    esd_config.led_e.pin | esd_config.led_f.pin;
    HAL_GPIO_Init(GPIOD, &gpio_init);

    gpio_init.Pin = esd_config.led_g.pin | esd_config.point.pin | esd_config.dot.pin;
    HAL_GPIO_Init(GPIOE, &gpio_init);
}

/**
 * @brief  Configures gpio-pins for joystick-gpio-port.
 */
void Joystick_GPIO_Init( void ) {
	__HAL_RCC_GPIOG_CLK_ENABLE();

	GPIO_InitTypeDef gpio_init_joy = { 0 };
	gpio_init_joy.Pin = joystick_config.pin_right | joystick_config.pin_down | joystick_config.pin_left | joystick_config.pin_up | joystick_config.pin_select;
	gpio_init_joy.Mode = GPIO_MODE_INPUT;
	gpio_init_joy.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(joystick_config.port, &gpio_init_joy);
}


int main(void) {

	// Hardware initialization
	HAL_Init();
	ESD_GPIO_Init();
	Joystick_GPIO_Init();
	timer_utils_init(); // Initialize our non-blocking timer

	// Module initialization
	esd_init(&esd_config);
	joystick_init(&joystick_config);

	// Application variables
	esd_position_t pos = 0;
	esd_digit_t digit = 0;
	uint8_t countdown_active = 0; // 0 = false, 1 = true

	// Time-keeping variables for non-blocking logic
	uint32_t last_joystick_read = 0;
	uint32_t last_countdown_tick = 0;

	while (1) {
		uint32_t current_ticks = timer_utils_get_ticks();

		// --- Handle Countdown Logic (every 1000ms if active) ---
		if (countdown_active && (current_ticks - last_countdown_tick >= 1000)) {
			if (digit > 0) {
				digit--;
			} else {
				countdown_active = 0; // Countdown finished
			}
			last_countdown_tick = current_ticks;
		}

		// --- Handle Input (every 50ms to debounce and prevent rapid fire) ---
		if (!countdown_active && (current_ticks - last_joystick_read >= 200)) {
			uint16_t state = joystick_read();

			if (state & joystick_config.pin_up) {
				digit = (digit + 1) % 10;
				last_joystick_read = current_ticks;
			} else if (state & joystick_config.pin_down) {
				digit = (digit + 9) % 10;
				last_joystick_read = current_ticks;
			} else if (state & joystick_config.pin_left) {
				pos = (pos + 4) % 5;
				last_joystick_read = current_ticks;
			} else if (state & joystick_config.pin_right) {
				pos = (pos + 1) % 5;
				last_joystick_read = current_ticks;
			} else if (state & joystick_config.pin_select) {
				countdown_active = 1;
				last_countdown_tick = current_ticks;
				last_joystick_read = current_ticks;
			}
		}

		// --- Handle Display Update (runs continuously) ---
		// This needs to be fast for multiplexing, so we don't delay it.
		// To display a number like "12", you would rapidly call:
		// esd_show_digit(1, ESD_POSITION_1);
		// esd_show_digit(2, ESD_POSITION_2);
		// For this simple example, we just show the same digit everywhere.
		esd_show_digit(digit, pos);
	}
}
