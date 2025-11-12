/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V2.1
 * @date    12-November-2025
 * @brief   Main program for the 00_Introduction project.
 *
 * @note    This project is a basic example that blinks the two user LEDs
 *          (green and red) present on the STM32F429I-DISC1 board.
 *          It demonstrates the basic initialization and control of GPIO pins.
 *
 * @section WIRING Wiring Instructions
 * The hardware used is internal to the development board and requires
 * no external wiring.
 * - **Green LED:** Connected to pin PG13.
 * - **Red LED:**   Connected to pin PG14.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void GPIO_Init(void);

/* Main program --------------------------------------------------------------*/

/**
 * @brief  Application entry point.
 * @retval int
 */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/

    // Initialize the Hardware Abstraction Layer (HAL)
    HAL_Init();

    // Configure the system clock
    SystemClock_Config();

    // Initialize the GPIO pins for the LEDs
    GPIO_Init();

    /* Infinite loop -----------------------------------------------------------*/
    while (1)
    {
        // Toggle the state of both LEDs (green and red)
        HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14);

        // Wait for 500 milliseconds
        HAL_Delay(500);
    }
}

/**
 * @brief  Configures the GPIO pins for the LEDs.
 * @note   Enables the clock for GPIOG port and configures pins
 *         PG13 and PG14 in output push-pull mode.
 * @retval None
 */
static void GPIO_Init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    // Enable the clock for the GPIOG port
    __HAL_RCC_GPIOG_CLK_ENABLE();

    // Configure the LED pins
    gpio_init_struct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP; // Push-Pull output mode
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;

    // Apply the configuration to the GPIOG port
    HAL_GPIO_Init(GPIOG, &gpio_init_struct);
}

/**
 * @brief  System Clock Configuration.
 * @note   This function is a basic configuration and might be more
 *         complex in real-world applications.
 * @retval None
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
