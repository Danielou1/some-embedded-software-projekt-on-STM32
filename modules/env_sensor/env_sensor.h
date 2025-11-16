/**
 ******************************************************************************
 * @file    env_sensor.h
 * @author  Danielou Mounsande
 * @version V1.0
 * @date    16-November-2025
 * @brief   Header file for the BME280 environment sensor module.
 *
 * @note    This module provides a simplified interface to the Bosch BME280
 *          environmental sensor, handling I2C communication and data
 *          compensation. It is configured to use the I2C1 peripheral.
 *
 * @section WIRING Wiring Instructions
 * To use this module, connect the BME280 sensor module to the microcontroller
 * as follows:
 * - **BME280 Sensor Module:**
 *   - VCC -> 3.3V
 *   - GND -> GND
 *   - SCL -> PB6 (Configured as I2C1_SCL)
 *   - SDA -> PB7 (Configured as I2C1_SDA)
 *   - CS (Chip Select) -> 3.3V (This selects the I2C interface on the sensor)
 *   - SDO (Data Out / Address) -> GND (This selects the primary I2C address 0x76)
 ******************************************************************************
 */

#ifndef ENV_SENSOR_H_
#define ENV_SENSOR_H_

#include "bme280/bme280.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Initializes the BME280 sensor and the I2C1 peripheral.
 * @note  This function configures the I2C1 peripheral, the corresponding GPIO
 *        pins (PB6, PB7), and initializes the Bosch BME280 driver with
 *        default oversampling and filter settings. It must be called before
 *        any other function in this module. See the WIRING section in the
 *        file header for connection details.
 * @retval BME280_OK (0) on success.
 * @retval Negative error code on failure (see bme280_defs.h).
 */
int8_t env_sensor_init(void);

/**
 * @brief Reads the latest temperature, pressure, and humidity data from the sensor.
 * @note  This function triggers a new measurement in "forced mode", waits for
 *        the measurement to complete, and then reads the compensated data.
 * @param comp_data Pointer to a `struct bme280_data` to store the compensated
 *                  sensor values. The data types (float/int) depend on the
 *                  BME280 driver configuration.
 * @retval BME280_OK (0) on success.
 * @retval Negative error code on failure (see bme280_defs.h).
 */
int8_t env_sensor_read_data(struct bme280_data *comp_data);

#endif /* ENV_SENSOR_H_ */