/**
 ******************************************************************************
 * @file    	gpio_blinker.c
 * @author		Danielou Mounsande
 * @version 	V1.1
 * @date		12-November-2025
 * @brief		Source file for a generic, non-blocking GPIO blinker module.
 ******************************************************************************
 */

#include "gpio_blinker/gpio_blinker.h"
#include "timer_utils/timer_utils.h"

/** @defgroup GPIO_Blinker_Private_Variables GPIO Blinker Private Variables
  * @{
  */
static GPIO_TypeDef* g_port;            /*!< Stores the GPIO port for the blinking pin. */
static uint16_t g_pin;                  /*!< Stores the GPIO pin number for the blinking pin. */
static uint32_t g_last_toggle_tick = 0; /*!< Stores the timestamp of the last toggle. */
/**
  * @}
  */

/**
 * @brief  Initializes the blinker module for a specific GPIO pin.
 * @note   This function configures the given pin as a push-pull output
 *         and enables its corresponding GPIO port clock. It should be
 *         called once before using the blinker.
 * @param  port: A pointer to the GPIO port (e.g., GPIOE).
 * @param  pin: The GPIO pin identifier (e.g., GPIO_PIN_11).
 * @retval None
 */
void gpio_blinker_init(GPIO_TypeDef* port, uint16_t pin) {
	g_port = port;
	g_pin = pin;

	// 1. Enable GPIO Clock for the given port
	if(g_port == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); }
	else if (g_port == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
	else if (g_port == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
	else if (g_port == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
	else if (g_port == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
	else if (g_port == GPIOF) { __HAL_RCC_GPIOF_CLK_ENABLE(); }
	else if (g_port == GPIOG) { __HAL_RCC_GPIOG_CLK_ENABLE(); }
	// Note: For H and I ports, add them here if needed.

	// 2. Configure GPIO Pin as a push-pull output
	GPIO_InitTypeDef gpio_init = {0};
	gpio_init.Pin = g_pin;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(g_port, &gpio_init);
}

/**
 * @brief  Updates the blinker state.
 * @note   This function must be called repeatedly in the main application loop
 *         to produce the blinking effect. It is non-blocking.
 * @param  interval_ms: The desired blinking interval in milliseconds.
 *         For example, 500ms will result in a 1Hz blink frequency (500ms ON, 500ms OFF).
 * @retval None
 */
void gpio_blinker_update(uint32_t interval_ms) {
	uint32_t current_ticks = timer_utils_get_ticks();

	// Check if the specified interval has passed since the last toggle
	if (current_ticks - g_last_toggle_tick >= interval_ms) {
		HAL_GPIO_TogglePin(g_port, g_pin);
		g_last_toggle_tick = current_ticks; // Update the last toggle timestamp
	}
}
