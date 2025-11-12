/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V3.2
 * @date    12-November-2025
 * @brief   Main program for the 03_LCD project.
 *
 * @note    This project demonstrates a non-blocking animation on the LCD screen.
 *          It displays a countdown from 10 to 0, synchronized with a
 *          decreasing bargraph. The timing is handled by the `timer_utils` module.
 *
 * @section WIRING Wiring Instructions
 * The LCD screen is integrated into the STM32F429I-DISC1 board and uses
 * the SPI5 peripheral. The connections are fixed as follows:
 *
 * | LCD Function      | STM32 Pin |
 * |:-----------------:|:---------:|
 * | SPI5_SCK (Clock)  | PF7       |
 * | SPI5_MISO (Data In) | PF8       |
 * | SPI5_MOSI (Data Out)| PF9       |
 * | CS (Chip Select)  | PC2       |
 * | DC (Data/Command) | PD13      |
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include "my_lcd/my_lcd.h"
#include "timer_utils/timer_utils.h"

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* Main Program --------------------------------------------------------------*/

/**
 * @brief  Application entry point.
 * @retval int
 */
int main(void) {
	/* Hardware and module initialization */
	HAL_Init();
	SystemClock_Config(); // Configure the system clock
	my_lcd_init();      // Initialize the custom LCD module
	timer_utils_init(); // Initialize the non-blocking timer

	/* Application variables */
	char buf[64];
	uint16_t bargraph_value = 1000; // Represents 100.0%
	int count = 10;

	uint32_t last_update = 0;

	/* Infinite loop */
	while (1) {
		// This is the non-blocking version of the bargraph animation.
		// The code inside this block only runs every 1000ms.
		if (timer_utils_get_ticks() - last_update >= 1000) {
			if (bargraph_value > 0) {
				// Prepare the text string for the countdown
				sprintf(buf, " %2d", count);
				if(count > 0)
				{
					count--;
				}

				// Redraw the bargraph and text
				my_lcd_draw_bargraph(0, 50, 240, 100, bargraph_value, BLUE, LIGHTGREY);
				my_lcd_draw_text_at_line(buf, 5, BLACK, 4, YELLOW);

				// Decrease the bargraph value for the next step
				bargraph_value -= 100;
			} else {
				// Reset the animation
				bargraph_value = 1000;
				count = 10;
			}
			// Update the time of the last animation step
			last_update = timer_utils_get_ticks();
		}
	}
}

/**
 * @brief  System Clock Configuration.
 * @retval None
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
