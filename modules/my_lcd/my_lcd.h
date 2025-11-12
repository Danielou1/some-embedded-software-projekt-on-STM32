/**
 ******************************************************************************
 * @file    my_lcd.h
 * @author  Danielou Mounsande
 * @version V1.0
 * @date    12-November-2025
 * @brief   Header file for a custom LCD utility module.
 *
 * @note    This module acts as a wrapper or extension for the base `lcd` module,
 *          providing additional high-level drawing functions like bargraphs.
 *          It simplifies the interface for application code (`main.c`).
 ******************************************************************************
 */

#ifndef MYLCD_H_
#define MYLCD_H_

/* Includes ------------------------------------------------------------------*/
#include "lcd/lcd.h" // Relies on the base lcd module

/* Public Function Prototypes ------------------------------------------------*/

/**
 * @brief Initializes the LCD screen.
 * @note  This function is a wrapper around the base `lcd_init()`.
 *        It handles the low-level initialization of the ILI9341 driver.
 */
void my_lcd_init(void);

/**
 * @brief Draws a text string at a given line on the screen.
 * @param text The text string to draw.
 * @param line The line number to draw the text on (0-indexed).
 * @param color The color of the text.
 * @param size The font size multiplier (1, 2, 3...).
 * @param background_color The background color of the text area.
 * @note  This function is a wrapper around the base `lcd_draw_text_at_line()`.
 */
void my_lcd_draw_text_at_line(const char* text, uint8_t line, uint16_t color, uint16_t size, uint16_t background_color);

/**
 * @brief Draws a bargraph on the screen.
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param width The total width of the bargraph.
 * @param height The height of the bargraph.
 * @param value The value to display, from 0 to 1000.
 * @param color The color of the filled part of the bar.
 * @param bg_color The color of the background (unfilled) part of the bar.
 */
void my_lcd_draw_bargraph(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t value, uint16_t color, uint16_t bg_color);

/**
 * @brief Draws a cross shape on the screen.
 * @note This is a non-blocking animation function. It should be called
 *       repeatedly in a loop to draw the cross step-by-step.
 */
void draw_kreuz(void);

#endif /* MYLCD_H_ */
