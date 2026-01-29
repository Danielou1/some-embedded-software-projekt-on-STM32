/**
 ******************************************************************************
 * @file    can_com.c
 * @author  Danielou Mounsande
 * @version V1.1
 * @date    25-January-2026
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
    // IMPORTANT: Set the instance BEFORE calling any HAL function
    hcan1.Instance = CAN1;

    // Standard initialization parameters as per working commit 2bdf9b5
    hcan1.Init.Prescaler = 21;
    hcan1.Init.Mode = CAN_MODE_LOOPBACK;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.AutoBusOff = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.AutoRetransmission = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    // Initialize the CAN peripheral (This calls HAL_CAN_MspInit)
    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        return -1; // Initialization failed
    }

    // Configure the filter to accept all messages
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
    sFilterConfig.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
    {
        return -2; // Filter configuration failed
    }

    // Start the CAN peripheral
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        return -3; // Start failed
    }

    // Activate the notifications for RX FIFO 0 and Errors
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | 
                                             CAN_IT_ERROR | 
                                             CAN_IT_BUSOFF | 
                                             CAN_IT_LAST_ERROR_CODE) != HAL_OK)
    {
        return -5; // Notification activation failed
    }

    return 0; // Success
}

/**
  * @brief CAN MSP Initialization
  * @param hcan: CAN handle pointer
  */
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hcan->Instance==CAN1)
  {
    /* 1. Enable peripheral clocks */
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /** 2. CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* 3. CAN1 interrupt Init */
    /* Priority 6 is safe for FreeRTOS syscalls */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  }
}

/**
  * @brief CAN MSP De-Initialization
  * @param hcan: CAN handle pointer
  */
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance==CAN1)
  {
    __HAL_RCC_CAN1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  }
}