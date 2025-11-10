/**
 ******************************************************************************
 * @file    	gpio_blinker.c
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		10.11.2025
 * @brief		A simple non-blocking GPIO blinker module.
 ******************************************************************************
 */

#include "gpio_blinker/gpio_blinker.h"
#include "timer_utils/timer_utils.h"

// Static variables to hold the configuration and state
static GPIO_TypeDef* g_port;
static uint16_t g_pin;
static uint32_t g_last_toggle_tick = 0;

/**
 * @brief Initializes the blinker with a specific GPIO pin.
 * @param port The GPIO port (e.g., GPIOE).
 * @param pin The GPIO pin (e.g., GPIO_PIN_11).
 */
void gpio_blinker_init(GPIO_TypeDef* port, uint16_t pin) {
	g_port = port;
	g_pin = pin;

	// 1. Enable GPIO Clock
	if(g_port == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
	else if (g_port == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
	else if (g_port == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
	else if (g_port == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
	else if (g_port == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
	else if (g_port == GPIOF) { __HAL_RCC_GPIOF_CLK_ENABLE(); }
	else if (g_port == GPIOG) { __HAL_RCC_GPIOG_CLK_ENABLE(); }
	// Add other ports if needed

	// 2. Configure GPIO Pin as Output
	GPIO_InitTypeDef gpio_init = {0};
	gpio_init.Pin = g_pin;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(g_port, &gpio_init);
}

/**
 * @brief Updates the blinker state. This function should be called
 *        repeatedly in the main loop.
 * @param interval_ms The blinking interval in milliseconds (e.g., 500 for a 1Hz blink).
 */
void gpio_blinker_update(uint32_t interval_ms) {
	uint32_t current_ticks = timer_utils_get_ticks();

	if (current_ticks - g_last_toggle_tick >= interval_ms) {
		HAL_GPIO_TogglePin(g_port, g_pin);
		g_last_toggle_tick = current_ticks;
	}
}
