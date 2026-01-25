/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "FreeRTOS.h"
#include "task.h"

/* External variables --------------------------------------------------------*/
extern CAN_HandleTypeDef hcan1;

/******************************************************************************/
/*            	  	    Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles SysTick Handler.
  * @note   We call HAL_IncTick here, then call the FreeRTOS Tick handler.
  */
void SysTick_Handler(void)
{
	HAL_IncTick();
    
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        extern void xPortSysTickHandler(void);
        xPortSysTickHandler();
    }
}

/* 
 * Note: SVC_Handler and PendSV_Handler are NOT defined here.
 * They are mapped directly to FreeRTOS port functions in FreeRTOSConfig.h.
 */

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/******************************************************************************/

void CAN1_RX0_IRQHandler(void)
{
  HAL_CAN_IRQHandler(&hcan1);
}