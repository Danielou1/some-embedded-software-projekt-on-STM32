/**
 ******************************************************************************
 * @file    my_lcd.c
 * @author  Danielou Mounsande
 * @version V1.0
 * @date    12-November-2025
 * @brief   Implementation file for a custom LCD utility module.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "my_lcd/my_lcd.h"
#include "timer_utils/timer_utils.h"
#include <lcd/lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Public function implementations -------------------------------------------*/

void my_lcd_init(void) {
    lcd_init();
}

void my_lcd_draw_text_at_line(const char* text, uint8_t line, uint16_t color, uint16_t size, uint16_t background_color) {
    lcd_draw_text_at_line(text, line, color, size, background_color);
}

void my_lcd_draw_bargraph(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
					uint16_t value, uint16_t color, uint16_t bg_color) {

    // Calculate the width of the filled portion of the bar
	uint16_t colored_width = (width * value) / 1000;
	uint16_t y1 = y + height;

    // Draw the background (unfilled) portion
	lcd_draw_rect(x + colored_width, y, x + width, y1, bg_color, 1);
	// Draw the foreground (filled) portion
	lcd_draw_rect(x, y, x + colored_width, y1, color, 1);
}

/**
 * @brief Draws a cross shape, one step at a time.
 * @note  This is a non-blocking animation function that uses a static variable
 *        to track its progress. It must be called repeatedly from a loop.
 */
void draw_kreuz(void) {
    static int i = 0;
    static uint32_t last_draw_tick = 0;

    // Limit drawing rate to prevent it from being too fast
    if (timer_utils_get_ticks() - last_draw_tick < 15) {
        return;
    }

    if (i < 100) {
        int centerX = ILI9341_SCREEN_WIDTH / 2;
        int centerY = ILI9341_SCREEN_HEIGHT / 2;
        lcd_draw_pixel(centerX, (centerY - 50) + i, BLACK); // Draw vertical line segment
        lcd_draw_pixel((centerX - 50) + i, centerY, BLACK); // Draw horizontal line segment
        i++;
    }
    last_draw_tick = timer_utils_get_ticks();
}
