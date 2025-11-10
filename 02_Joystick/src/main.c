/**
 ******************************************************************************
 * @file    	main.c
 * @author      Danielou Mounsande & Danielle Ndjensi
 * @version 	V1.0
 * @date        24.04.2024
 * @brief		Vorlage mit dem 8BIT SEG und dem Joystick
 ******************************************************************************
 */

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include "esd/esd.h"
#include "delay_utils/delay_utils.h"
#include "joystick/joystick.h"

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


/**
 * @brief  Hauptfunktion, die die Logik für die Anzeige auf dem 8-Segment-Display und die Steuerung über den Joystick enthält.
 * @param  None
 * @return Null bei Erfolg und jeder andere Wert im Fehlerfall
 */

int main(void) {

	//Hardware initialization
	HAL_Init();
	ESD_GPIO_Init();
	Joystick_GPIO_Init();

	// Initialisierung des 8-Segment-Displays und des Joysticks
	esd_init(&esd_config);
	joystick_init(&joystick_config);

	// Initialisierung der Position und des Digits (Zahl) für die Anzeige
	esd_position_t pos = 0;
	esd_digit_t digit = 0;


	while (1) {
		uint16_t state = joystick_read();

		// Überprüfen, ob die Joystick-Richtungstasten gedrückt werden

		// Joystick UP: Erhöhe das Digit (Zahl) um 1
		if (state & joystick_config.pin_up) {
			digit = (digit + 1) % (10);

		// Joystick DOWN: Verringere das Digit (Zahl) um 1
		} else if (state & joystick_config.pin_down) {
			digit = (digit + 9) % (10);

		// Joystick LEFT: Ändere die Anzeigeposition nach links
		} else if (state & joystick_config.pin_left) {
			pos = (pos + 4) % (5);

		// Joystick RIGHT: Ändere die Anzeigeposition nach rechts
		} else if (state & joystick_config.pin_right) {
			pos = (pos + 1) % (5);

			// Joystick SELECT: Starte den Countdown von der aktuellen Zahl
		} else if (state & joystick_config.pin_select) {

			for (int temp = digit; temp >= 0; temp--) {
				esd_show_digit(temp, pos);
				utils_delay_ms(1000);
				digit = temp;

			}
		}

		// Zeige das aktuelle Digit (Zahl) an der aktuellen Position auf dem Display
		esd_show_digit(digit, pos);
		// Kurze Verzögerung
		utils_delay_ms(200);
	}
}
