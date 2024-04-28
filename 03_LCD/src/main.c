/**
 /**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande & Danielle Ndjensi
 * @version 	V1.0
 * @date		25.04.2024
 * @brief  		Verwendung des LCD-Bildschirms für die schrittweise Anzeige von Zahlen
 ******************************************************************************
 */

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include "my_lcd/my_lcd.h"
#include <stdio.h>
#include <math.h>

/**
 * @brief  Hauptfunktion, die die Logik für die Anzeige der Nummern 1 bis 10 auf dem LCD enthält.
 * @param  None
 * @return Null bei Erfolg und jeder andere Wert im Fehlerfall
 */

int main(void) {
	/*Hardware initialization*/
	HAL_Init();

	/*Initialisierung des LCD-Displays*/
	lcd_init();

	// Puffer für die Textanzeige auf dem LCD-Display
	char buf[64];
	int count = 10;
	while (1) {
		// Schleife zur Erstellung der Balkengraphen

		/**for (uint16_t zahl = 1000; zahl >= 10; zahl -= 100) {
			sprintf(buf, " %4d", count);
			count--;
			my_lcd_draw_bargraph(0, 0, 240, 100, zahl, BLACK, BLUE);
			lcd_draw_text_at_line(buf, 5, BLACK, 4, YELLOW);
			HAL_Delay(1000);
		}
		count = 10;*/

		draw_kreuz();
	}

}
