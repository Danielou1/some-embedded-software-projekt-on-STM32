/**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande
 * @version 	V3.0
 * @date		12-November-2025
 * @brief		Main program for the 06_Blinky_Dot project.
 *
 * @note    This project fulfills task 3 of "Abgabe 3". It demonstrates
 *          how to use a hardware timer in PWM mode to generate a low-frequency
 *          signal (1Hz) to blink an LED without CPU intervention in the main loop.
 *
 * @section WIRING Wiring Instructions
 * This project uses the 8-segment LED display board.
 * - The "Dot" segment on **PE11** is controlled by the PWM signal from TIM1_CH2.
 * - Digit selectors **PD0, PD1, PD14, PD15** are enabled to make the dot visible.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "my_lcd/my_lcd.h"
#include "esd/esd.h"
#include "pwm_dimmer/pwm_dimmer.h"

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);

/* Main Program --------------------------------------------------------------*/

/**
 * @brief  Application entry point.
 * @retval int
 */
int main(void)
{
	// Hardware and module initialization
	HAL_Init();
	SystemClock_Config();
	SystemCoreClockUpdate(); // Update the SystemCoreClock variable
	my_lcd_init();

	// Define and initialize the ESD module for pin configuration
	ESD_Config_t esd_config = {
		.cntl1 = {GPIOD, GPIO_PIN_0}, .cntl2 = {GPIOD, GPIO_PIN_1},
		.cntl3 = {GPIOD, GPIO_PIN_14}, .cntl4 = {GPIOD, GPIO_PIN_15},
		.led_a = {GPIOE, GPIO_PIN_8}, .led_b = {GPIOE, GPIO_PIN_9},
		.led_c = {GPIOE, GPIO_PIN_10}, .led_d = {GPIOE, GPIO_PIN_12},
		.led_e = {GPIOE, GPIO_PIN_13}, .led_f = {GPIOE, GPIO_PIN_14},
		.led_g = {GPIOE, GPIO_PIN_15}, .point = {GPIOE, GPIO_PIN_11},
		.dot = {GPIOE, GPIO_PIN_11}
	};
	esd_init(&esd_config);

	// Configure PWM for the "Dot" segment (PE11, TIM1_CH2)
	PWM_Dimmer_Config_t pwm_config = {
		.tim_instance = TIM1, .tim_channel = TIM_CHANNEL_2,
		.gpio_port = GPIOE, .gpio_pin = GPIO_PIN_11,
		.gpio_af = GPIO_AF1_TIM1
	};
	// Initialize PWM to generate a 1Hz signal with 50% duty cycle
	pwm_dimmer_init(&pwm_config, 1, 0.5f);

	// Draw an informational message on the LCD
	my_lcd_draw_text_at_line("Blinky Dot (PWM)", 4, BLACK, 2, WHITE);
	my_lcd_draw_text_at_line("50% Duty Cycle", 6, BLACK, 2, WHITE);
	my_lcd_draw_text_at_line("PE11, Timer: TIM1_CH2", 7, BLACK, 2, WHITE);

	while(1) {
		// Keep all digits on the 7-segment display enabled
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);

		// No code needed here. The PWM hardware handles the blinking automatically.
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
