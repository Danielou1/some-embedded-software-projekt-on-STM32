/**
  ******************************************************************************
  * @file    	main.c
  * @author		Florian von Zabiensky
  * @version 	V1.0
  * @date		14.03.2019
  * @brief  	Template using the LCD
  ******************************************************************************
*/
#include <stdio.h>

#include <lcd/lcd.h>
#include <joystick/joystick.h>
#include "stm32f4xx.h"


int main(void)
{
	char x[10];

	sprintf(x, "%d", 1);
	HAL_Init();

	/* Initialization of the LCD */
	lcd_init();


	/* Draw text to LCD */
	lcd_draw_text_at_line("   Hello World!", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Welcome to", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       PES", 7, BLACK, 2, WHITE);

	joystick_init();

	while(1) {
		uint16_t joy = joystick_read();

		if(joy & JOYSTICK_PIN_RIGHT)
		{
			lcd_draw_text_at_line("   RIGHT       ", 1, BLACK, 2, WHITE);
		}
		else
		{
			lcd_draw_text_at_line("               ", 1, BLACK, 2, WHITE);
		}
		if(joy & JOYSTICK_PIN_UP)
		{
			lcd_draw_text_at_line("   UP          ", 2, BLACK, 2, WHITE);
		}
		else
		{
			lcd_draw_text_at_line("               ", 2, BLACK, 2, WHITE);
		}
		if(joy & JOYSTICK_PIN_DOWN)
		{
			lcd_draw_text_at_line("   DOWN        ", 3, BLACK, 2, WHITE);
		}
		else
		{
			lcd_draw_text_at_line("               ", 3, BLACK, 2, WHITE);
		}
		if(joy & JOYSTICK_PIN_LEFT)
		{
			lcd_draw_text_at_line("   LEFT        ", 4, BLACK, 2, WHITE);
		}
		else
		{
			lcd_draw_text_at_line("               ", 4, BLACK, 2, WHITE);
		}
		if(joy & JOYSTICK_PIN_SELECT)
		{
			lcd_draw_text_at_line("   SELECT      ", 5, BLACK, 2, WHITE);
		}
		else
		{
			lcd_draw_text_at_line("               ", 5, BLACK, 2, WHITE);
		}
	}
}
