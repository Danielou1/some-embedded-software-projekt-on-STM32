/**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande
 * @version 	V2.0
 * @date		reviewed on 15-November-2025
 * @brief		Main program for the P1_Fan_Control project.
 *
 * @note    This final version implements the full PI control loop.
 *          - A potentiometer sets the target RPM (Soll-Drehzahl).
 *          - The tacho reader measures the actual RPM (Ist-Drehzahl).
 *          - A PI controller adjusts the PWM duty cycle to match the
 *            actual RPM to the target RPM.
 *          - A periodic timer (TIM3) triggers the control loop every 100ms.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdbool.h>
#include "my_lcd/my_lcd.h"
#include "tacho_reader/tacho_reader.h"
#include "pwm_dimmer/pwm_dimmer.h"
#include "potis/potis.h"
#include "pi_controller/pi_controller.h"

/* Private defines -----------------------------------------------------------*/
#define KP_START 0.001f   // Initial Proportional Gain
#define KI_START 0.0005f  // Initial Integral Gain
#define DT_SECONDS 0.1f   // Control loop interval in seconds (100ms)

/* Global variables ----------------------------------------------------------*/
TIM_HandleTypeDef g_tim3_handle;
volatile bool g_run_controller_flag = false;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void TIM3_Init(void);

/* Main Program --------------------------------------------------------------*/

int main(void)
{
	// --- Initialization ---
	HAL_Init();
	SystemClock_Config();
	SystemCoreClockUpdate();
	my_lcd_init();
	potis_init(1);
	tacho_reader_init();
	TIM3_Init(); // Initialize the 100ms control loop timer

	// Configure and initialize the PWM for the fan
	PWM_Dimmer_Config_t fan_pwm_config = {
		.tim_instance = TIM1,
		.tim_channel = TIM_CHANNEL_2,
		.gpio_port = GPIOE,
		.gpio_pin = GPIO_PIN_11,
		.gpio_af = GPIO_AF1_TIM1,
		.gpio_mode = GPIO_MODE_AF_OD
	};
	pwm_dimmer_init(&fan_pwm_config, 25000, 0.0f);

	// Initialize the PI controller
	// Output is clamped between 20% and 100% duty cycle
	pi_controller_init(KP_START, KI_START, 0.2f, 1.0f);

	// --- Application variables ---
	char buffer[32];
	float duty_cycle = 0.0f;

	my_lcd_draw_text_at_line("Fan Controller", 0, BLACK, 2, WHITE);
	my_lcd_draw_text_at_line("PI Control Active", 1, DARKGREY, 2, WHITE);

	while(1) {
		// Wait for the control loop timer flag
		if (g_run_controller_flag) {
			g_run_controller_flag = false; // Reset flag

			// --- 1. Get Setpoint and Actual Value ---
			uint16_t poti_val = potis_get_val(0);
			float setpoint_rpm = (poti_val / 4095.0f) * 5000.0f; // Map 0-4095 to 0-5000 RPM
			float actual_rpm = (float)tacho_reader_get_rpm();

			// --- 2. Update Controller ---
			duty_cycle = pi_controller_update(setpoint_rpm, actual_rpm, DT_SECONDS);

			// --- 3. Set Actuator ---
			pwm_dimmer_set_duty_cycle(duty_cycle);

			// --- 4. Display Info ---
			sprintf(buffer, "Soll: %4.0f RPM", setpoint_rpm);
			my_lcd_draw_text_at_line(buffer, 4, DARKGREEN, 2, WHITE);

			sprintf(buffer, "Ist : %4.0f RPM", actual_rpm);
			my_lcd_draw_text_at_line(buffer, 5, DARKCYAN, 2, WHITE);

			sprintf(buffer, "Duty: %3d%%", (int)(duty_cycle * 100));
			my_lcd_draw_text_at_line(buffer, 8, BLUE, 2, WHITE);
		}
		// Can use __WFI() here to save power while waiting for interrupt
	}
}

/**
 * @brief  Initializes TIM3 to generate an update interrupt every 100ms.
 * @retval None
 */
static void TIM3_Init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    // APB1 Timer Clock is 84MHz.
    // To get a 10Hz (100ms) update frequency, we need to count 8,400,000 ticks.
    // We can use a prescaler and a period to achieve this.
    // E.g., Prescaler = 8399, Period = 999
    // Update Freq = 84MHz / ((8399+1) * (999+1)) = 84,000,000 / (8400 * 1000) = 10 Hz.
    g_tim3_handle.Instance = TIM3;
    g_tim3_handle.Init.Prescaler = 8399;
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim3_handle.Init.Period = 999;
    g_tim3_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&g_tim3_handle);

    // Start the timer in interrupt mode
    HAL_TIM_Base_Start_IT(&g_tim3_handle);

    // Enable the TIM3 interrupt in the NVIC
    HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
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
