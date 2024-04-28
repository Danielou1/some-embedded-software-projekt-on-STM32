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

/**
 * @brief  Hauptfunktion, die die Logik für die Anzeige auf dem 8-Segment-Display und die Steuerung über den Joystick enthält.
 * @param  None
 * @return Null bei Erfolg und jeder andere Wert im Fehlerfall
 */

int main(void) {

	//Hardware initialization
	HAL_Init();

	// Initialisierung des 8-Segment-Displays und des Joysticks
	esd_init();
	joystick_init();

	// Initialisierung der Position und des Digits (Zahl) für die Anzeige
	esd_position_t pos = 0;
	esd_digit_t digit = 0;


	while (1) {
		uint16_t state = utils_gpio_port_read(JOYSTICK_PORT);

		// Überprüfen, ob die Joystick-Richtungstasten gedrückt werden

		// Joystick UP: Erhöhe das Digit (Zahl) um 1
		if ((state & JOYSTICK_PIN_UP) == 0) { //JOYSTICK UP
			digit = (digit + 1) % (10);

		// Joystick DOWN: Verringere das Digit (Zahl) um 1
		} else if ((state & JOYSTICK_PIN_DOWN) //JOYSTICK DOWN
				== 0) {
			digit = (digit + 9) % (10);

		// Joystick LEFT: Ändere die Anzeigeposition nach links
		} else if ((state & JOYSTICK_PIN_LEFT) // JOYSTICK LEFT
				== 0) {
			pos = (pos + 4) % (5);

		// Joystick RIGHT: Ändere die Anzeigeposition nach rechts
		} else if ((state & JOYSTICK_PIN_RIGHT) //JOYSTICK RIGHT
				== 0) {
			pos = (pos + 1) % (5);

			// Joystick SELECT: Starte den Countdown von der aktuellen Zahl
		} else if ((state & JOYSTICK_PIN_SELECT) // JOYSTICK PRESS & COUNTDOWN
				== 0) {

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
