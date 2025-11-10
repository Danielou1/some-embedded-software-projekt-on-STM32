/**
 ******************************************************************************
 * @file    	potis.h
 * @author		Danielou Mounsande
 * @version 	V1.0
 * @date		10.11.2025
 * @brief		Module for reading potentiometers using the ADC in polling mode.
 ******************************************************************************
 */

#ifndef MODULES_POTIS_POTIS_H_
#define MODULES_POTIS_POTIS_H_

#include "stm32f4xx.h"

/**
 * @brief Configuration structure for a single potentiometer.
 */
typedef struct {
	ADC_TypeDef* adc_instance;
	uint32_t adc_channel;
	GPIO_TypeDef* gpio_port;
	uint16_t gpio_pin;
} Potis_Config_t;


/**
 * @brief Initializes the ADC for a specific potentiometer.
 * @param config Pointer to the configuration struct for the potentiometer.
 */
void potis_init(Potis_Config_t* config);

/**
 * @brief Reads the raw ADC value for the initialized potentiometer.
 * @return The raw ADC value (0-4095 for 12-bit resolution).
 */
uint32_t potis_get_value(void);

/**
 * @brief Reads the ADC value and converts it to millivolts.
 * @return The voltage in millivolts (0-3300mV).
 */
uint32_t potis_get_millivolts(void);


#endif /* MODULES_POTIS_POTIS_H_ */
