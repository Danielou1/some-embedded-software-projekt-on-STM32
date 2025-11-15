/**
 ******************************************************************************
 * @file    	pwm_dimmer.c
 * @author		Danielou Mounsande
 * @version 	V2.0
 * @date		12-November-2025
 * @brief		Source file for a generic, frequency-based PWM dimmer module.
 ******************************************************************************
 */

#include "pwm_dimmer/pwm_dimmer.h"

/** @defgroup PWM_Dimmer_Private_Variables PWM Dimmer Private Variables
  * @{
  */
static TIM_HandleTypeDef g_tim_handle;              /*!< Handle for the timer configuration. */
static PWM_Dimmer_Config_t g_dimmer_config;         /*!< Copy of the user-provided hardware configuration. */
static uint32_t g_pwm_period;                       /*!< Stores the calculated PWM period for duty cycle calculations. */
/**
  * @}
  */

/**
 * @brief  Initializes a timer channel in PWM mode for a specific pin and frequency.
 * @note   This function calculates the optimal prescaler and period to achieve the
 *         desired frequency, then configures the hardware and starts the PWM signal.
 * @param  config: Pointer to a `PWM_Dimmer_Config_t` struct containing all hardware parameters.
 * @param  frequency: The desired PWM frequency in Hz.
 * @param  initial_duty_cycle: The initial duty cycle, from 0.0f (0%) to 1.0f (100%).
 * @retval None
 */
void pwm_dimmer_init(PWM_Dimmer_Config_t* config, uint32_t frequency, float initial_duty_cycle) {
	g_dimmer_config = *config;

	// --- GPIO Configuration ---
	if(g_dimmer_config.gpio_port == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
	else if (g_dimmer_config.gpio_port == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
	else if (g_dimmer_config.gpio_port == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
	else if (g_dimmer_config.gpio_port == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
	else if (g_dimmer_config.gpio_port == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
	else if (g_dimmer_config.gpio_port == GPIOF) { __HAL_RCC_GPIOF_CLK_ENABLE(); }
	else if (g_dimmer_config.gpio_port == GPIOG) { __HAL_RCC_GPIOG_CLK_ENABLE(); }

	GPIO_InitTypeDef gpio_init = {0};
	gpio_init.Pin = g_dimmer_config.gpio_pin;
	gpio_init.Mode = g_dimmer_config.gpio_mode;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_init.Alternate = g_dimmer_config.gpio_af;
	HAL_GPIO_Init(g_dimmer_config.gpio_port, &gpio_init);

	// --- Timer Configuration ---
	if(g_dimmer_config.tim_instance == TIM1) { __HAL_RCC_TIM1_CLK_ENABLE(); }

	// --- Prescaler and Period Calculation ---
	// Get the timer's source clock frequency. For TIM1/TIM8 it's typically HCLK or HCLK/2 depending on APB2 prescaler.
	// On this board, with APB2 prescaler at /2, the timer clock is HCLK = 168MHz.
	uint32_t timer_clock = SystemCoreClock;
	uint32_t prescaler = (timer_clock / (frequency * 65536)) + 1;
	uint32_t period = timer_clock / (prescaler * frequency);
	g_pwm_period = period; // Store the calculated period

	// --- Timer Base Configuration ---
	g_tim_handle.Instance = g_dimmer_config.tim_instance;
	g_tim_handle.Init.Prescaler = prescaler - 1;
	g_tim_handle.Init.Period = period - 1;
	g_tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	g_tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	g_tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_PWM_Init(&g_tim_handle);

	// --- PWM Channel Configuration ---
	uint32_t initial_pulse = (uint32_t)(g_pwm_period * initial_duty_cycle);
	TIM_OC_InitTypeDef tim_oc_init = {0};
	tim_oc_init.OCMode = TIM_OCMODE_PWM1;
	tim_oc_init.Pulse = initial_pulse;
	tim_oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim_oc_init.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&g_tim_handle, &tim_oc_init, g_dimmer_config.tim_channel);

	// --- Start PWM ---
	HAL_TIM_PWM_Start(&g_tim_handle, g_dimmer_config.tim_channel);

	if(g_dimmer_config.tim_instance == TIM1 || g_dimmer_config.tim_instance == TIM8)
	{
		__HAL_TIM_MOE_ENABLE(&g_tim_handle);
	}
}

/**
 * @brief  Sets the duty cycle of the PWM signal.
 * @param  duty_cycle: The new duty cycle, from 0.0f (0%) to 1.0f (100%).
 * @retval None
 */
void pwm_dimmer_set_duty_cycle(float duty_cycle) {
	if (duty_cycle < 0.0f) duty_cycle = 0.0f;
	if (duty_cycle > 1.0f) duty_cycle = 1.0f;

	uint32_t pulse = (uint32_t)(g_pwm_period * duty_cycle);
	__HAL_TIM_SET_COMPARE(&g_tim_handle, g_dimmer_config.tim_channel, pulse);
}
