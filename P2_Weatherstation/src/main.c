/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V2.1
 * @brief   Main program body for the P2 Weather Station - Debug Version.
 ******************************************************************************
 */

#include "lcd/lcd.h"
#include "stm32f4xx.h"
#include "env_sensor/env_sensor.h"
#include "can_com/can_com.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdbool.h>

/* Private types -------------------------------------------------------------*/
typedef struct {
    float temperature;
    float pressure;
    float humidity;
} WeatherData_t;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void vSensorTask(void *pvParameters);
void vCanTask(void *pvParameters);
void vUiTask(void *pvParameters);

/* Global variables ----------------------------------------------------------*/
TaskHandle_t xSensorTaskHandle = NULL;
TaskHandle_t xCanTaskHandle    = NULL;
TaskHandle_t xUiTaskHandle     = NULL;
QueueHandle_t xWeatherQueue    = NULL;

/* Shared data (Protected by tasks) */
volatile WeatherData_t g_latest_weather = {0};
volatile uint32_t g_heartbeat = 0;

int main(void)
{
	HAL_Init();
    SystemClock_Config();

	lcd_init();
	lcd_fill_screen(WHITE);
	lcd_draw_text_at_line("P2: RTOS Debug", 1, BLACK, 2, WHITE);

	if (env_sensor_init() != BME280_OK)
	{
		lcd_draw_text_at_line("BME280 Init Failed!", 3, RED, 2, WHITE);
		/* Note: We don't block here to let the rest of the system run if possible */
	}

    int can_res = can_com_init();
    if (can_res != 0)
	{
        char err[32];
        sprintf(err, "CAN Error: %d", can_res);
		lcd_draw_text_at_line(err, 4, RED, 2, WHITE);
	}

    /* Queue Creation */
    xWeatherQueue = xQueueCreate(5, sizeof(WeatherData_t));

    if (xWeatherQueue != NULL)
    {
        /* Increase stack size to 512 to support sprintf with floats (%f) */
        BaseType_t r1 = xTaskCreate(vSensorTask, "Sensor", 512, NULL, 2, &xSensorTaskHandle);
        BaseType_t r2 = xTaskCreate(vCanTask,    "CAN",    512, NULL, 3, &xCanTaskHandle);
        BaseType_t r3 = xTaskCreate(vUiTask,     "UI",     512, NULL, 1, &xUiTaskHandle);

        if (r1 != pdPASS || r2 != pdPASS || r3 != pdPASS)
        {
            lcd_draw_text_at_line("Task Create FAIL!", 6, RED, 2, WHITE);
            while(1);
        }

        lcd_draw_text_at_line("Starting Scheduler...", 15, BLUE, 1, WHITE);
        vTaskStartScheduler();
    }

	while (1);
}

void vSensorTask(void *pvParameters)
{
    struct bme280_data sensor_raw;
    WeatherData_t data;

    for (;;)
    {
        /* If hardware is not connected, this function should return an error
           rather than looping infinitely. */
        if (env_sensor_read_data(&sensor_raw) == BME280_OK)
        {
            data.temperature = (float)sensor_raw.temperature;
            data.pressure    = (float)sensor_raw.pressure / 100.0f;
            data.humidity    = (float)sensor_raw.humidity;
            g_latest_weather = data;
            xQueueSend(xWeatherQueue, &data, 0); 
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vCanTask(void *pvParameters)
{
    WeatherData_t received;
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8];
    uint32_t txMailbox;

    txHeader.StdId = 0x400;
    txHeader.RTR   = CAN_RTR_DATA;
    txHeader.IDE   = CAN_ID_STD;
    txHeader.DLC   = 4;
    txHeader.TransmitGlobalTime = DISABLE;

    for (;;)
    {
        if (xQueueReceive(xWeatherQueue, &received, portMAX_DELAY) == pdPASS)
        {
            int16_t temp = (int16_t)(received.temperature * 100);
            txData[0] = (temp >> 8) & 0xFF;
            txData[1] = temp & 0xFF;
            /* Real transmission (Loopback active) */
            HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox);
        }
    }
}

void vUiTask(void *pvParameters)
{
    char buf[32];
    for (;;)
    {
        g_heartbeat++;
        
        sprintf(buf, "T: %.2f C", g_latest_weather.temperature);
        lcd_draw_text_at_line(buf, 6, BLACK, 2, WHITE);

        sprintf(buf, "H: %.2f %%", g_latest_weather.humidity);
        lcd_draw_text_at_line(buf, 8, BLACK, 2, WHITE);

        sprintf(buf, "Tick: %lu", g_heartbeat);
        lcd_draw_text_at_line(buf, 13, GREEN, 2, WHITE);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

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
