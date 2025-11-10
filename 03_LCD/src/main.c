/**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande & Danielle Ndjensi
 * @version 	V2.0
 * @date		10.11.2025
 * @brief  		Verwendung des LCD-Bildschirms für die schrittweise Anzeige von Zahlen (non-blocking)
 ******************************************************************************
 */

/* Includes */
#include "stm32f4xx.h"
#include <stdio.h>
#include "lcd/lcd.h"
#include "my_lcd/my_lcd.h"
#include "timer_utils/timer_utils.h"


int main(void) {
	/* Hardware initialization */
	HAL_Init();
	lcd_init();
	timer_utils_init();

	/* Application variables */
	char buf[64];
	uint16_t bargraph_value = 1000;
	int count = 10;

	uint32_t last_update = 0;

	while (1) {
		// This is the non-blocking version of the bargraph animation
		if (timer_utils_get_ticks() - last_update >= 1000) {
			if (bargraph_value >= 10) {
				sprintf(buf, " %4d", count);
				count--;

				my_lcd_draw_bargraph(0, 0, 240, 100, bargraph_value, BLACK, BLUE);
				lcd_draw_text_at_line(buf, 5, BLACK, 4, YELLOW);

				bargraph_value -= 100;
			} else {
				// Reset the animation
				bargraph_value = 1000;
				count = 10;
			}
			last_update = timer_utils_get_ticks();
		}


		// The non-blocking cross drawing function can be called here as well.
		// It will draw one step of the cross animation without blocking the bargraph animation.
		// draw_kreuz();
	}
}
