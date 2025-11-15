/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#ifdef USE_RTOS_SYSTICK
#include <cmsis_os.h>
#endif
#include "stm32f4xx_it.h"
#include "tacho_reader/tacho_reader.h"
#include "timer_utils/timer_utils.h"
#include <stdbool.h>

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef g_tim3_handle;
extern TIM_HandleTypeDef htim6; // From timer_utils
extern volatile bool g_run_controller_flag;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/******************************************************************************/

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&g_tim3_handle);
}

/**
  * @brief This function handles TIM6 global interrupt and DAC1, DAC2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  // This is the entry point for the Tacho Pin interrupt (PC7).
  // It calls the generic HAL handler, which will check the interrupt flag
  // and then call the user callback HAL_GPIO_EXTI_Callback.
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  // Check if the interrupt was triggered by the tacho pin (PC7)
  if (GPIO_Pin == GPIO_PIN_7)
  {
    // Handle the tacho pulse logic
    tacho_reader_handle_pulse();
  }
}

/**
  * @brief  Period elapsed callback in non-blocking mode.
  * @note   This is the single callback for all timers.
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // Check if the interrupt comes from our controller timer (TIM3)
  if (htim->Instance == TIM3)
  {
    // Set the flag to run the PI controller logic in the main loop
    g_run_controller_flag = true;
  }
  // Check if the interrupt comes from the timer_utils timer (TIM6)
  else if (htim->Instance == TIM6)
  {
	// Call the handler from the timer_utils module
	timer_utils_handle_tick();
  }
}
