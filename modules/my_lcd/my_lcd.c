#include "my_lcd/my_lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void my_lcd_draw_bargraph(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
					uint16_t value, uint16_t color, uint16_t bg_color) {

	uint16_t colored_width = (width * value)/1000;
	uint16_t y1 = y + height;
	lcd_draw_rect(x + colored_width, y, x + width, y1, bg_color, 1);
	lcd_draw_rect(x, y, x + colored_width, y1, color, 1);
}
void draw_kreuz() {
	 // Calcul des coordonnées du centre de l'écran
	    int centerX = 100 / 2;
	    int centerY = 100 / 2;

	    // Dessin de la croix centrée
	    for (int i = 0; i < 100; i++) {
	        lcd_draw_pixel(centerX, centerY - 50 + i, BLACK); // Vertical
	        lcd_draw_pixel(centerX - 50 + i, centerY, BLACK); // Horizontal


	        //lcd_draw_pixel(centerX - 50 + i, centerY - 50 + i, BLACK); //diagonal
	        //lcd_draw_pixel(centerX + 50 - i, centerY - 50 + i, BLACK); //diagonal

	        HAL_Delay(150);
	    }
}

