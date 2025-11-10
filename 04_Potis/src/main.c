/**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		10.11.2025
 * @brief  		Reads a potentiometer using the ADC and displays it on the LCD.
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include <stdio.h>
#include "lcd/lcd.h"
#include "timer_utils/timer_utils.h"
#include "potis/potis.h"


// Configuration for the potentiometer on PA7
Potis_Config_t poti1_config = {
	.adc_instance = ADC1,
	.adc_channel = ADC_CHANNEL_7,
	.gpio_port = GPIOA,
	.gpio_pin = GPIO_PIN_7
};


int main(void)
{
	// Hardware and module initialization
	HAL_Init();
	lcd_init();
	timer_utils_init();
	potis_init(&poti1_config);

	// Application variables
	char buf[64];
	uint32_t last_update = 0;

	// Initial screen message
	lcd_draw_text_at_line("Potentiometer ADC", 4, BLACK, 2, WHITE);


	while(1) {
		// Perform this check every 250ms
		if(timer_utils_get_ticks() - last_update >= 250)
		{
			// Read the value in millivolts
			uint32_t mv_val = potis_get_millivolts();

			// Format the string
			sprintf(buf, "Poti 1: %4lu mV", mv_val);

			// Clear the line by drawing a white rectangle
			lcd_draw_rect(0, 6*20, 320, 20, WHITE, 1);
			// Draw the new text
			lcd_draw_text_at_line(buf, 6, BLACK, 2, WHITE);

			// Update the time keeper
			last_update = timer_utils_get_ticks();
		}
	}
}
