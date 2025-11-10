/**
 ******************************************************************************
 * @file    	potis.c
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		10.11.2025
 * @brief		Module for reading potentiometers using the ADC in polling mode.
 ******************************************************************************
 */

#include "potis/potis.h"

// Static handle for the ADC peripheral
static ADC_HandleTypeDef g_adc_handle;
// Static storage for the configuration
static Potis_Config_t g_potis_config;


/**
 * @brief Initializes the ADC for a specific potentiometer.
 * @param config Pointer to the configuration struct for the potentiometer.
 */
void potis_init(Potis_Config_t* config) {
	// Store the configuration
	g_potis_config = *config;

	// --- GPIO Initialization ---
	// 1. Enable GPIO Clock
	if(g_potis_config.gpio_port == GPIOA) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
	} else if (g_potis_config.gpio_port == GPIOB) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
	} else if (g_potis_config.gpio_port == GPIOC) {
		__HAL_RCC_GPIOC_CLK_ENABLE();
	} // Add other ports if needed

	// 2. Configure GPIO Pin as Analog
	GPIO_InitTypeDef gpio_init = {0};
	gpio_init.Pin = g_potis_config.gpio_pin;
	gpio_init.Mode = GPIO_MODE_ANALOG;
	gpio_init.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(g_potis_config.gpio_port, &gpio_init);


	// --- ADC Initialization ---
	// 1. Enable ADC Clock
	if(g_potis_config.adc_instance == ADC1) {
		__HAL_RCC_ADC1_CLK_ENABLE();
	} else if (g_potis_config.adc_instance == ADC2) {
		__HAL_RCC_ADC2_CLK_ENABLE();
	} else if (g_potis_config.adc_instance == ADC3) {
		__HAL_RCC_ADC3_CLK_ENABLE();
	}

	// 2. Configure ADC Handle
	g_adc_handle.Instance = g_potis_config.adc_instance;
	g_adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4; // 84MHz / 4 = 21MHz (max 36MHz)
	g_adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
	g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	g_adc_handle.Init.ScanConvMode = DISABLE;
	g_adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	g_adc_handle.Init.ContinuousConvMode = ENABLE;
	g_adc_handle.Init.NbrOfConversion = 1;
	g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	g_adc_handle.Init.DiscontinuousConvMode = DISABLE;
	g_adc_handle.Init.NbrOfDiscConversion = 0;
	g_adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;

	HAL_ADC_Init(&g_adc_handle);

	// 3. Configure ADC Channel
	ADC_ChannelConfTypeDef adc_channel_config = {0};
	adc_channel_config.Channel = g_potis_config.adc_channel;
	adc_channel_config.Rank = 1;
	adc_channel_config.SamplingTime = ADC_SAMPLETIME_84CYCLES;

	HAL_ADC_ConfigChannel(&g_adc_handle, &adc_channel_config);

	// 4. Start ADC
	HAL_ADC_Start(&g_adc_handle);
}

/**
 * @brief Reads the raw ADC value for the initialized potentiometer.
 * @return The raw ADC value (0-4095 for 12-bit resolution).
 */
uint32_t potis_get_value(void) {
	// Workaround for Overrun Detection issue in continuous mode
	HAL_ADC_Start(&g_adc_handle);

	if (HAL_ADC_PollForConversion(&g_adc_handle, 10) == HAL_OK) {
		return HAL_ADC_GetValue(&g_adc_handle);
	}
	return 0;
}

/**
 * @brief Reads the ADC value and converts it to millivolts.
 * @return The voltage in millivolts (0-3300mV).
 */
uint32_t potis_get_millivolts(void) {
	uint32_t raw_value = potis_get_value();
	// Using integer arithmetic to avoid floats: (value * 3300) / 4095
	return (raw_value * 3300) / 4095;
}
