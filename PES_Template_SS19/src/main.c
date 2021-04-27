/**
  ******************************************************************************
  * @file    	main.c
  * @author		Florian von Zabiensky
  * @version 	V1.0
  * @date		14.03.2019
  * @brief  	Template using the LCD
  ******************************************************************************
*/


#include <lcd/lcd.h>
#include "stm32f4xx.h"


int main(void)
{
	HAL_Init();

	/* Initialization of the LCD */
	lcd_init();

	/* Draw text to LCD */
	lcd_draw_text_at_line("   Hello World!", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Welcome to", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       PES", 7, BLACK, 2, WHITE);

	while(1) {

	}
}
