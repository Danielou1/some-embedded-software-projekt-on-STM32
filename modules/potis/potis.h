/**
 ******************************************************************************
 * @file    potis.h
 * @author  Danielou Mounsande
 * @version V2.1
 * @date    12-November-2025
 * @brief   Header file for the potentiometer ADC reader module.
 *
 * @note    This module provides functions to initialize and read values from
 *          two potentiometers. It supports both polling and DMA-based reading modes.
 *
 * @section WIRING Wiring Instructions
 * To use this module, connect two 10k Ohm potentiometers to the microcontroller as follows:
 * - **Potentiometer 1 (POTI_1):**
 *   - Wiper (middle pin) -> PA6 (ADC1_IN6)
 *   - One side pin -> 3.3V
 *   - Other side pin -> GND
 *
 * - **Potentiometer 2 (POTI_2):**
 *   - Wiper (middle pin) -> PA7 (ADC1_IN7)
 *   - One side pin -> 3.3V
 *   - Other side pin -> GND
 ******************************************************************************
 */

#ifndef INC_POTIS_H_
#define INC_POTIS_H_

#include "stm32f4xx_hal.h"

/* Public Defines ------------------------------------------------------------*/

/** @defgroup Poti_Public_Defines Potentiometer Public Defines
  * @{
  */
#define POTI_1 0U /*!< Identifier for Potentiometer 1 (connected to PA6 / ADC1_IN6) */
#define POTI_2 1U /*!< Identifier for Potentiometer 2 (connected to PA7 / ADC1_IN7) */
/**
  * @}
  */

/* Public Enums --------------------------------------------------------------*/

/**
 * @brief Defines the operating modes for the potentiometer ADC conversion.
 */
typedef enum {
    POTI_MODE_POLLING, /**< ADC conversion is initiated manually and the CPU waits for it to complete. */
    POTI_MODE_DMA      /**< ADC conversions run continuously in the background using DMA. */
} PotiMode;


/* Public Function Prototypes ------------------------------------------------*/

/**
 * @brief Initializes the hardware for reading the potentiometers.
 * @param mode The desired operating mode (Polling or DMA).
 * @note  This function configures the ADC1 peripheral, the corresponding GPIO pins (PA6, PA7),
 *        and, if applicable, the DMA2 Stream 0 for continuous data transfer.
 *        It calls HAL_ADC_MspInit internally. See the WIRING section in the file header for connection details.
 */
void potis_init(PotiMode mode);

/**
 * @brief Reads the analog value from the specified potentiometer.
 * @param poti_num The potentiometer to read (use POTI_1 or POTI_2).
 * @return uint32_t The 12-bit ADC conversion value (0-4095).
 * @note  In POLLING mode, this function triggers a new conversion and waits for the result.
 *        In DMA mode, it returns the latest averaged value from the DMA buffer without
 *        triggering a new conversion.
 */
uint32_t potis_get_val(uint8_t poti_num);

#endif /* INC_POTIS_H_ */