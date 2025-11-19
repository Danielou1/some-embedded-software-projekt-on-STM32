/**
 ******************************************************************************
 * @file    can_com.c
 * @author  Danielou Mounsande
 * @version V1.0
 * @date    18-November-2025
 * @brief   Implementation file for the CAN communication module.
 ******************************************************************************
 */

#include "can_com.h"

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

/* Public Function Implementations -------------------------------------------*/

/**
 * @brief Initializes the CAN1 peripheral and configures it for Loopback Mode.
 */
int can_com_init(void)
{
    // De-initialize the CAN peripheral first to reset it completely.
    if (HAL_CAN_DeInit(&hcan1) != HAL_OK)
    {
        return -4; // Add a new error code for DeInit failure
    }

    hcan1.Instance = CAN1;
    hcan1.Init.Prescaler = 21;
    hcan1.Init.Mode = CAN_MODE_LOOPBACK; // Set to Loopback mode for testing
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        return -1; // Initialization failed
    }

    // After Init, we need to configure the filter.
    // Without a filter, the CAN peripheral will not receive any messages.
    CAN_FilterTypeDef sFilterConfig;

    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14; // Relevant for dual CAN mode

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
        return -2; // Filter configuration failed
    }


    // Start the CAN peripheral
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        return -3; // Start failed
    }

    return 0; // Success
}


/**
  * @brief CAN MSP Initialization
  *        This function configures the hardware resources:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hcan: CAN handle pointer
  * @retval None
  */
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hcan->Instance==CAN1)
  {
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  }
}

/**
  * @brief CAN MSP De-Initialization
  * @param hcan: CAN handle pointer
  * @retval None
  */
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance==CAN1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
  }
}
