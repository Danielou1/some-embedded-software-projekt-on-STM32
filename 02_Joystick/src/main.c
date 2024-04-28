/**
 ******************************************************************************
 * @file    	main.c
 * @author      Danielou Mounsande & Danielle Djentsi
 * @version 	V1.0
 * @date        18.04.2024
 * @brief
 ******************************************************************************
 */

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include "esd/esd.h"
#include "delay_utils/delay_utils.h"
#include "joystick/joystick.h"

int main(void) {

	//Hardware initialization
	HAL_Init();
	esd_init();
	joystick_init();

	esd_position_t pos = 0;
	esd_digit_t digit = 0;


	while (1) {
		if ((utils_gpio_port_read(JOYSTICK_PORT) & JOYSTICK_PIN_UP) == 0) { //JOYSTICK UP
			digit = (digit + 1) % (10);

		} else if ((utils_gpio_port_read(JOYSTICK_PORT) & JOYSTICK_PIN_DOWN) //JOYSTICK DOWN
				== 0) {
			digit = (digit + 9) % (10);

		} else if ((utils_gpio_port_read(JOYSTICK_PORT) & JOYSTICK_PIN_LEFT) // JOYSTICK LEFT
				== 0) {
			pos = (pos + 4) % (5);

		} else if ((utils_gpio_port_read(JOYSTICK_PORT) & JOYSTICK_PIN_RIGHT) //JOYSTICK RIGHT
				== 0) {
			pos = (pos + 1) % (5);

		} else if ((utils_gpio_port_read(JOYSTICK_PORT) & JOYSTICK_PIN_SELECT) // JOYSTICK PRESS & COUNTDOWN
				== 0) {

			for (int temp = digit; temp >= 0; temp--) {
				esd_show_digit(temp, pos);
				utils_delay_ms(1000);
				digit = temp;

			}
		}

		esd_show_digit(digit, pos);
		utils_delay_ms(200);
	}
}
