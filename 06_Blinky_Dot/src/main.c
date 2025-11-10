/**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		10.11.2025
 * @brief  		Blinks the "Dot" segment of the 7-segment display.
 ******************************************************************************
 */

#include "stm32f4xx.h"
#include "lcd/lcd.h"
#include "timer_utils/timer_utils.h"
#include "gpio_blinker/gpio_blinker.h"

// Function to initialize the control pins for the 7-segment display
void ESD_Control_GPIO_Init(void) {
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init = {0};
    gpio_init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_14 | GPIO_PIN_15; // CNTL1, CNTL2, CNTL3, CNTL4
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &gpio_init);
}

int main(void)
{
	// Hardware and module initialization
	HAL_Init();
	lcd_init();
	timer_utils_init();
	ESD_Control_GPIO_Init(); // Initialize control pins

	// Initialize the blinker for the "Dot" segment (PE11)
	gpio_blinker_init(GPIOE, GPIO_PIN_11);

	// Draw an informational message on the LCD
	lcd_draw_text_at_line("Blinky Dot", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("Dot segment (PE11)", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("should be blinking...", 7, BLACK, 2, WHITE);

	while(1) {
		// Enable all digits on the 7-segment display for the dot to be visible
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);

		// Update the blinker state non-blockingly
		gpio_blinker_update(500); // Blink every 500ms (1Hz frequency)
	}
}
