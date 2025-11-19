#include "lcd/lcd.h"
#include "stm32f4xx.h"
#include "env_sensor/env_sensor.h"
#include "can_com/can_com.h"
#include <stdio.h>
#include <stdbool.h>

// Private function prototypes
void SystemClock_Config(void);

// Global flag for our CAN loopback test
volatile bool g_can_loopback_success = false;

/**
  * @brief  Rx Fifo 0 message pending callback in non blocking mode
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    // Get the message
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
    {
        // Check if it's our test message (ID 0x123)
        if (rxHeader.StdId == 0x123)
        {
            g_can_loopback_success = true;
        }
        // Later, we will add logic here to process incoming sensor data
    }
}


int main(void)
{
	HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

	/* Initialization of the LCD */
	lcd_init();
	lcd_fill_screen(WHITE);
	lcd_draw_text_at_line("P2: Wetterstation", 2, BLACK, 2, WHITE);

	/* Initialization of the BME280 Sensor */
	if (env_sensor_init() != BME280_OK)
	{
		lcd_draw_text_at_line("BME280 Init Failed!", 4, RED, 2, WHITE);
		while(1);
	}
	else
	{
		lcd_draw_text_at_line("BME280 Initialized", 4, GREEN, 2, WHITE);
	}

	/* CAN Initialization */
    int can_init_status = can_com_init();
	if (can_init_status != 0)
	{
        char error_msg[32];
        sprintf(error_msg, "CAN Init Failed! Code: %d", can_init_status);
		lcd_draw_text_at_line(error_msg, 6, RED, 2, WHITE);
		while(1);
	}
	else
	{
		lcd_draw_text_at_line("CAN Initialized", 6, GREEN, 2, WHITE);
	}

    /* Activate the CAN RX FIFO 0 message pending interrupt */
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    {
        lcd_draw_text_at_line("CAN IRQ Activate Failed!", 6, RED, 2, WHITE);
        while(1);
    }

    HAL_Delay(500);

#if 0 // Set to 1 to re-enable the CAN loopback test, 0 for normal operation
    /* --- Perform Loopback Test (Preserved for reference) --- */
    lcd_draw_text_at_line("Performing CAN Test...", 8, BLACK, 2, WHITE);

    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[] = {'T', 'E', 'S', 'T'};
    uint32_t txMailbox;

    txHeader.StdId = 0x123; // Test ID
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.IDE = CAN_ID_STD;
    txHeader.DLC = 4; // 4 bytes of data
    txHeader.TransmitGlobalTime = DISABLE;

    // Send the message
    if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox) != HAL_OK)
    {
        lcd_draw_text_at_line("CAN Send Failed!", 10, RED, 2, WHITE);
    }

    HAL_Delay(100); // Wait a short moment for the loopback to complete

    // Check the result
    if (g_can_loopback_success)
    {
        lcd_draw_text_at_line("CAN Loopback OK!", 10, GREEN, 2, WHITE);
    }
    else
    {
        lcd_draw_text_at_line("CAN Test FAILED!", 10, RED, 2, WHITE);
    }

	while(1)
	{
		// Loop forever after the test
	}
#endif

    // Clear init messages for normal operation
    lcd_draw_rect(0, 4 * 16, 240, 12 * 16, WHITE, 1);

	while(1)
	{
		// Final application logic will go here:
        // 1. Read sensor data from BME280
        // 2. Send sensor data via CAN every second
        // 3. Update LCD with data from selected node (via joystick)
        HAL_Delay(1000);
	}
}

/**
 * @brief  System Clock Configuration.
 * @note   This configures the system clock to 168 MHz and the APB1 clock to 42 MHz.
 *         This is crucial for the CAN baud rate calculation.
 * @retval None
 */
void SystemClock_Config(void)
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