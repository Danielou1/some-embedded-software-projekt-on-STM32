/**
 /**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande & Danielle Djenzi
 * @version 	V1.0
 * @date		28.10.2022
 * @brief  	LCD
 ******************************************************************************
 */

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include "my_lcd/my_lcd.h"
#include <stdio.h>
#include <math.h>


int main(void) {
	HAL_Init();

	lcd_init();

	char buf[64];
	int Count = 10;
	while (1) {

		/**for (uint16_t zahl = 1000; zahl >= 10; zahl -= 100) {
			sprintf(buf, " %4d", Count);
			Count--;
			my_lcd_draw_bargraph(0, 0, 240, 100, zahl, BLACK, BLUE);
			lcd_draw_text_at_line(buf, 6, BLACK, 4, YELLOW);
			HAL_Delay(1000);
		}
		Count = 10;*/

		draw_kreuz();
	}

}
