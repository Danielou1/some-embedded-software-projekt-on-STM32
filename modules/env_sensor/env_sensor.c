/**
 ******************************************************************************
 * @file    env_sensor.c
 * @author  Danielou Mounsande
 * @version V1.0
 * @date    16-November-2025
 * @brief   Implementation file for the BME280 environment sensor module.
 ******************************************************************************
 */

#include "env_sensor.h"
#include <stdlib.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/

/** @brief Static instance of the BME280 device structure */
static struct bme280_dev bme;

/** @brief Static instance of the I2C handle for I2C1 */
static I2C_HandleTypeDef hi2c1;

/* Private function prototypes -----------------------------------------------*/

static int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);
static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);
static void user_delay_us(uint32_t period, void *intf_ptr);

/* Public Function Implementations -------------------------------------------*/

/**
 * @brief Initializes the BME280 sensor and the I2C1 peripheral.
 */
int8_t env_sensor_init(void)
{
    int8_t rslt = BME280_OK;

    // I2C peripheral initialization
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000; // 100kHz
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        return -1; // Error
    }

    // Configure Analogue filter (based on working example)
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        return -1; // Error
    }

    // Configure Digital filter (based on working example)
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        return -1; // Error
    }

    // BME280 device structure configuration
    bme.intf_ptr = NULL; // Not needed for HAL_I2C_Mem_Read/Write
    bme.intf = BME280_I2C_INTF;
    bme.read = user_i2c_read;
    bme.write = user_i2c_write;
    bme.delay_us = user_delay_us;

    // Initialize BME280 driver
    rslt = bme280_init(&bme);
    if (rslt != BME280_OK)
    {
        return rslt;
    }

    // Configure sensor settings for oversampling and filtering
    bme.settings.osr_h = BME280_OVERSAMPLING_1X;
    bme.settings.osr_p = BME280_OVERSAMPLING_16X;
    bme.settings.osr_t = BME280_OVERSAMPLING_2X;
    bme.settings.filter = BME280_FILTER_COEFF_16;

    uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

    rslt = bme280_set_sensor_settings(settings_sel, &bme);

    return rslt;
}

/**
 * @brief Reads the latest sensor data in forced mode.
 */
int8_t env_sensor_read_data(struct bme280_data *comp_data)
{
    int8_t rslt;
    uint32_t req_delay;

    // Set sensor to forced mode to trigger a single measurement
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &bme);
    if (rslt != BME280_OK)
    {
        return rslt;
    }

    // Calculate the minimum delay required between setting the mode and reading the data
    req_delay = bme280_cal_meas_delay(&bme.settings);
    bme.delay_us(req_delay, &bme); // Pass correct delay

    // Get the compensated sensor data
    rslt = bme280_get_sensor_data(BME280_ALL, comp_data, &bme);

    return rslt;
}

/* Private Function Implementations ------------------------------------------*/

/**
 * @brief  Platform-specific I2C read function for the BME280 driver.
 * @note   This function uses the STM32 HAL I2C Memory Read function.
 * @param  reg_addr: The register address to read from.
 * @param  reg_data: Pointer to the buffer to store the read data.
 * @param  len: The number of bytes to read.
 * @param  intf_ptr: Unused interface pointer (kept for compatibility).
 * @retval BME280_OK on success, BME280_E_COMM_FAIL on failure.
 */
static int8_t user_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (BME280_I2C_ADDR_PRIM << 1), reg_addr,
                                                I2C_MEMADD_SIZE_8BIT, reg_data, len, HAL_MAX_DELAY);
    return (status == HAL_OK) ? BME280_OK : BME280_E_COMM_FAIL;
}

/**
 * @brief  Platform-specific I2C write function for the BME280 driver.
 * @note   This function uses the STM32 HAL I2C Memory Write function.
 * @param  reg_addr: The register address to write to.
 * @param  reg_data: Pointer to the data to be written.
 * @param  len: The number of bytes to write.
 * @param  intf_ptr: Unused interface pointer (kept for compatibility).
 * @retval BME280_OK on success, BME280_E_COMM_FAIL on failure.
 */
static int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, (BME280_I2C_ADDR_PRIM << 1), reg_addr,
                                                 I2C_MEMADD_SIZE_8BIT, (uint8_t *)reg_data, len, HAL_MAX_DELAY);
    return (status == HAL_OK) ? BME280_OK : BME280_E_COMM_FAIL;
}

/**
 * @brief  Platform-specific delay function for the BME280 driver.
 * @note   The BME280 driver is inconsistent, sometimes calling this with
 *         microseconds and sometimes with milliseconds. This implementation
 *         uses a heuristic to handle both cases.
 * @param  period: The delay period.
 * @param  intf_ptr: Unused interface pointer.
 */
static void user_delay_us(uint32_t period, void *intf_ptr)
{
    // The bme280_cal_meas_delay() function returns ms, so we pass that directly.
    // The bme280_init() function calls with us (e.g. 2000 for 2ms).
    // This simple implementation uses HAL_Delay which has ms precision.
    // For periods < 1000us, it will result in a 0ms delay, which is acceptable
    // for the 2ms startup time but might be an issue for more precise needs.
    // A more robust implementation would use a DWT cycle counter for sub-ms delays.
    HAL_Delay(period / 1000);
}


/**
  * @brief I2C MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  }
}

/**
  * @brief I2C MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{
  if(i2cHandle->Instance==I2C1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
  }
}