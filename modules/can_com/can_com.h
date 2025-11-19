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
 * @brief Initializes the CAN1 peripheral and configures filters.
 * @note  This function configures GPIOs PA11 and PA12 for CAN1, sets the
 *        baud rate to 125 kBit/s, configures the necessary filters to
 *        receive weather data, and starts the CAN peripheral.
 * @retval 0 on success, -1 on failure.
 */
int can_com_init(void);


#endif /* CAN_COM_H_ */
