/**
 ******************************************************************************
 * @file    can_com.h
 * @author  Danielou Mounsande
 * @version V1.0
 * @date    16-November-2025
 * @brief   Header file for the CAN communication module.
 ******************************************************************************
 */

#ifndef CAN_COM_H_
#define CAN_COM_H_

#include "stm32f4xx_hal.h"

extern CAN_HandleTypeDef hcan1;

/**
 * @brief  Initializes the CAN1 peripheral for communication.
 * @note   This function performs a full initialization of the CAN1 peripheral.
 *         It configures the following:
 *         - **GPIO Pins**: PB8 (CAN1_RX) and PB9 (CAN1_TX).
 *         - **Mode**: The CAN controller is set to @c CAN_MODE_LOOPBACK for self-testing without an external transceiver.
 *         - **Baud Rate**: The communication speed is set to 125 kBit/s, assuming a 42 MHz APB1 clock.
 *         - **Filter**: A default filter is configured to accept all standard CAN messages (Filter Bank 0).
 *
 *         The function also handles de-initialization, low-level MSP initialization (via HAL),
 *         filter configuration, and starting the CAN peripheral.
 *
 * @retval 0 on success.
 * @retval -1 if HAL_CAN_Init fails.
 * @retval -2 if HAL_CAN_ConfigFilter fails.
 * @retval -3 if HAL_CAN_Start fails.
 * @retval -4 if HAL_CAN_DeInit fails.
 */
int can_com_init(void);


#endif /* CAN_COM_H_ */
