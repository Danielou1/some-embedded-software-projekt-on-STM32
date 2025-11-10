#include "my_lcd/my_lcd.h"
#include "timer_utils/timer_utils.h"
#include <lcd/lcd.h> // Added for LCD_WIDTH and LCD_HEIGHT
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

void draw_kreuz(void) {
    static int i = 0;
    static uint32_t last_draw_tick = 0;

    if (timer_utils_get_ticks() - last_draw_tick < 15) {
        return; // Not time to draw yet
    }

    if (i < 100) {
        int centerX = ILI9341_SCREEN_WIDTH / 2;
        int centerY = ILI9341_SCREEN_HEIGHT / 2;
        lcd_draw_pixel(centerX, (centerY - 50) + i, BLACK); // Vertical
        lcd_draw_pixel((centerX - 50) + i, centerY, BLACK); // Horizontal
        i++;
    }
    last_draw_tick = timer_utils_get_ticks();
}
