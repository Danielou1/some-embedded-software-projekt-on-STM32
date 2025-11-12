/**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande
 * @version 	V2.1
 * @date		reviewed on 12-November-2025
 * @brief		Main program for the 07_Dimming_Dot project.
 *
 * @note    This project fulfills the requirement of task 5 from "Abgabe 3".
 *          It demonstrates controlling an LED's brightness via a potentiometer,
 *          using a combination of the ADC (`potis` module) and a Timer in
 *          PWM mode (`pwm_dimmer` module).
 *
 * @section WIRING Wiring Instructions
 * This project uses multiple peripherals:
 * - **8-Segment Display:**
 *   - The "Dot" segment on **PE11** is controlled by the PWM signal.
 *   - Digit selectors **PD0, PD1, PD14, PD15** are enabled.
 * - **Potentiometer:**
 *   - Potentiometer 1 is connected to **PA6** (ADC1, Channel 6).
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include "my_lcd/my_lcd.h"
#include "timer_utils/timer_utils.h"
#include "pwm_dimmer/pwm_dimmer.h"
#include "esd/esd.h"
#include "potis/potis.h"

/* Private constants ---------------------------------------------------------*/
#define PWM_FREQUENCY 1000 // Use a 1kHz PWM frequency for smooth dimming
#define ADC_MAX_VALUE 4095 // 12-bit ADC resolution

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
	timer_utils_init();
	potis_init(POTI_MODE_POLLING); // Initialize the potentiometer module in polling mode

	// Define and initialize the ESD module
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
	pwm_dimmer_init(&pwm_config, PWM_FREQUENCY, 0.0f); // Start with 0% duty cycle

	// Application variables
	char lcd_buf[64];
	uint32_t last_lcd_update = 0;

	while(1) {
		// Keep all digits on the 7-segment display enabled
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_SET);

		// --- Main Logic: Read ADC and set PWM Duty Cycle ---
		uint16_t adc_value = potis_get_val(POTI_1);
		float logical_duty_cycle = (float)adc_value / (float)ADC_MAX_VALUE;
		
		// Invert the duty cycle for the active-low LED hardware
		float inverted_duty_cycle = 1.0f - logical_duty_cycle;
		pwm_dimmer_set_duty_cycle(inverted_duty_cycle);

		// --- UI Logic: Update LCD periodically ---
		if (timer_utils_get_ticks() - last_lcd_update >= 200) // Update LCD every 200ms
		{
			// Display the logical (non-inverted) duty cycle for an intuitive UI
			uint16_t percentage = (uint16_t)(logical_duty_cycle * 100);
			sprintf(lcd_buf, "Duty Cycle: %3d%%", percentage);
			my_lcd_draw_text_at_line("Dimming with ", 4, BLACK, 2, WHITE);
			my_lcd_draw_text_at_line("Potentiometer", 6, BLACK, 2, WHITE);
			my_lcd_draw_text_at_line(lcd_buf, 8, BLACK, 2, WHITE);
			last_lcd_update = timer_utils_get_ticks();
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
