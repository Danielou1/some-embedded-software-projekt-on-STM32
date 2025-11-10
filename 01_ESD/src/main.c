/**
  ******************************************************************************
  * @file    	main.c
  * @author		Danielou Mounsande & Danielle Ndjensi
  * @version 	V1.0
  * @date       25.04.2024
  ******************************************************************************
*/

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include "esd/esd.h"
#include "delay_utils/delay_utils.h"

// Create the configuration struct with the original pin values
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

/**
 * @brief Initializes the GPIO pins for the 7-segment display.
 */
void ESD_GPIO_Init(void) {
    // Enable clock for GPIO ports D and E
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_MEDIUM;

    // Initialize GPIOD pins
    gpio_init.Pin = esd_config.cntl1.pin | esd_config.cntl2.pin | esd_config.cntl3.pin | esd_config.cntl4.pin |
                    esd_config.led_a.pin | esd_config.led_b.pin | esd_config.led_c.pin | esd_config.led_d.pin |
                    esd_config.led_e.pin | esd_config.led_f.pin;
    HAL_GPIO_Init(GPIOD, &gpio_init);

    // Initialize GPIOE pins
    gpio_init.Pin = esd_config.led_g.pin | esd_config.point.pin | esd_config.dot.pin;
    HAL_GPIO_Init(GPIOE, &gpio_init);
}


/**
 * @brief Hauptfunktion, die die Logik für die Anzeige auf dem ESD enthält.
 * @param None
 * @return Null bei Erfolg und jeder andere Wert im Fehlerfall
 */

int main(void) {
	/*Hardware initialization*/
	HAL_Init();
	ESD_GPIO_Init();

	/*Initialisierung des 8-Segment-Displays*/
	esd_init(&esd_config);

	// Arrays für die Darstellung der Ziffern und Positionen
	esd_digit_t digitArray[10] = { ESD_DIGIT_0, ESD_DIGIT_1, ESD_DIGIT_2,
			ESD_DIGIT_3, ESD_DIGIT_4, ESD_DIGIT_5, ESD_DIGIT_6, ESD_DIGIT_7,
			ESD_DIGIT_8, ESD_DIGIT_9 };
	esd_position_t positionArray[5] = { ESD_POSITION_1, ESD_POSITION_2,
			ESD_POSITION_3, ESD_POSITION_4, ESD_POSITION_ALL };

	// Zähler für die Auswahl der Ziffer und Position
	int digitCounter = 9;
	int posCounter = 0;

	while (1) {
		// Ziffer und Position anzeigen
		esd_show_digit(digitArray[digitCounter], positionArray[posCounter]);
		utils_delay_ms(1000);
		digitCounter--;

		// Überprüfen, ob der Zähler für die Ziffer unter 0 ist
		if(digitCounter < 0){
			posCounter++;
			digitCounter = 9;
		}
		if(posCounter == 5) posCounter = 0;

	}
}
