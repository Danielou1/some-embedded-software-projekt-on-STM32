/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V3.5 (Interrupt-Driven Edition)
 * @brief   Weather Station - Fully reactive UI based on interrupts.
 ******************************************************************************
 */

#include "lcd/lcd.h"
#include "stm32f4xx.h"
#include "env_sensor/env_sensor.h"
#include "can_com/can_com.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>
#include <stdbool.h>

/* Private types -------------------------------------------------------------*/
typedef struct {
    int32_t temperature_x100;
    uint32_t pressure_pa;
    uint32_t humidity_x100;
} WeatherData_t;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void vSensorTask(void *pvParameters);
void vCanTask(void *pvParameters);
void vUiTask(void *pvParameters);
void vLcdDrawTextSafe(char *text, uint8_t line, uint16_t color, uint8_t size, uint16_t bgcolor);

/* Global handles ------------------------------------------------------------*/
TaskHandle_t xSensorTaskHandle = NULL;
TaskHandle_t xCanTaskHandle    = NULL;
TaskHandle_t xUiTaskHandle     = NULL;

QueueHandle_t xWeatherQueue    = NULL;
SemaphoreHandle_t xLcdMutex    = NULL; 
SemaphoreHandle_t xDataReadySync = NULL; 

/* Shared data */
volatile WeatherData_t g_latest_weather = {0};
volatile uint32_t g_heartbeat = 0;
volatile uint32_t g_last_can_rx_tick = 0;
volatile bool g_sensor_data_valid = false;
volatile bool g_can_status_ok = false;

/**
  * @brief  The application's main entry point.
  */
int main(void)
{
    HAL_Init();
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    SystemClock_Config();

    lcd_init();
    lcd_fill_screen(WHITE);
    lcd_draw_text_at_line("P2: Reactive RTOS", 1, BLACK, 2, WHITE);

    /* Initial Sensor Probe */
    if (env_sensor_init() == BME280_OK) {
        lcd_draw_text_at_line("BME280: OK", 3, GREEN, 2, WHITE);
    } else {
        lcd_draw_text_at_line("BME280: OFF", 3, RED, 2, WHITE);
    }

    /* Use the modular CAN driver */
    if (can_com_init() == 0) {
        lcd_draw_text_at_line("CAN Bus: OK", 4, GREEN, 2, WHITE);
    } else {
        lcd_draw_text_at_line("CAN Bus: OFF", 4, RED, 2, WHITE);
    }

    /* RTOS Objects */
    xLcdMutex = xSemaphoreCreateMutex();
    xDataReadySync = xSemaphoreCreateBinary();
    xWeatherQueue = xQueueCreate(5, sizeof(WeatherData_t));

    if (xLcdMutex != NULL && xWeatherQueue != NULL)
    {
        xTaskCreate(vSensorTask, "Sensor", 512, NULL, 2, &xSensorTaskHandle);
        xTaskCreate(vCanTask,    "CAN",    512, NULL, 3, &xCanTaskHandle);
        xTaskCreate(vUiTask,     "UI",     512, NULL, 1, &xUiTaskHandle);

        vTaskStartScheduler();
    }
    while (1);
}

/**
 * @brief Callback for CAN Reception.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
    {
        if (rxHeader.StdId == 0x400)
        {
            /* Decode data */
            int16_t temp = (int16_t)((rxData[0] << 8) | rxData[1]);
            uint16_t hum = (uint16_t)((rxData[2] << 8) | rxData[3]);
            uint32_t press = (uint32_t)((rxData[4] << 24) | (rxData[5] << 16) | (rxData[6] << 8) | rxData[7]);

            g_latest_weather.temperature_x100 = temp;
            g_latest_weather.humidity_x100 = hum;
            g_latest_weather.pressure_pa = press * 10;

            g_last_can_rx_tick = xTaskGetTickCountFromISR();
            g_can_status_ok = true;

            /* Wake up the UI task */
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xDataReadySync, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

/**
 * @brief Callback for CAN Errors.
 * This handles "CAN Bus: OFF" instantly using interrupts.
 */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    g_can_status_ok = false;
    
    /* Wake up the UI task immediately to show the error */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xDataReadySync, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vLcdDrawTextSafe(char *text, uint8_t line, uint16_t color, uint8_t size, uint16_t bgcolor)
{
    if (xLcdMutex == NULL) return;
    if (xSemaphoreTake(xLcdMutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        lcd_draw_text_at_line(text, line, color, size, bgcolor);
        xSemaphoreGive(xLcdMutex);
    }
}

void vSensorTask(void *pvParameters)
{
    struct bme280_data sensor_raw;
    WeatherData_t data;
    for (;;) {
        if (env_sensor_read_data(&sensor_raw) == BME280_OK) {
            data.temperature_x100 = (int32_t)(sensor_raw.temperature * 100);
            data.pressure_pa      = (uint32_t)(sensor_raw.pressure);
            data.humidity_x100    = (uint32_t)(sensor_raw.humidity * 100);
            g_sensor_data_valid = true;
            xQueueSend(xWeatherQueue, &data, 0); 
        } else {
            g_sensor_data_valid = false;
            /* Wake up UI to show sensor error */
            xSemaphoreGive(xDataReadySync); 
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vCanTask(void *pvParameters)
{
    WeatherData_t received;
    CAN_TxHeaderTypeDef txHeader = { .StdId = 0x400, .RTR = CAN_RTR_DATA, .IDE = CAN_ID_STD, .DLC = 8 };
    uint8_t txData[8];
    uint32_t txMailbox;
    for (;;) {
        if (xQueueReceive(xWeatherQueue, &received, portMAX_DELAY) == pdPASS) {
            int16_t temp = (int16_t)(received.temperature_x100);
            uint16_t hum = (uint16_t)(received.humidity_x100);
            uint32_t press = received.pressure_pa / 10;
            txData[0] = (temp >> 8) & 0xFF; txData[1] = temp & 0xFF;
            txData[2] = (hum >> 8) & 0xFF;  txData[3] = hum & 0xFF;
            txData[4] = (press >> 24) & 0xFF; txData[5] = (press >> 16) & 0xFF;
            txData[6] = (press >> 8) & 0xFF;  txData[7] = press & 0xFF;
            
            HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox);
        }
    }
}

void vUiTask(void *pvParameters)
{
    char buf[32];
    for (;;) {
        /* WAIT FOREVER for an event (No more polling!) */
        xSemaphoreTake(xDataReadySync, portMAX_DELAY);
        g_heartbeat++;

        /* Verify CAN Watchdog (still needed for silent timeouts) */
        if ((xTaskGetTickCount() - g_last_can_rx_tick) > pdMS_TO_TICKS(2500)) {
            g_can_status_ok = false;
        }

        /* Update Labels */
        vLcdDrawTextSafe(g_sensor_data_valid ? "BME280: OK  " : "BME280: OFF ", 3, g_sensor_data_valid ? GREEN : RED, 2, WHITE);
        vLcdDrawTextSafe(g_can_status_ok ? "CAN Bus: OK " : "CAN Bus: OFF", 4, g_can_status_ok ? GREEN : RED, 2, WHITE);

        if (g_can_status_ok && g_sensor_data_valid) 
        {
            vLcdDrawTextSafe("                    ", 12, WHITE, 2, WHITE);
            vLcdDrawTextSafe("                    ", 14, WHITE, 2, WHITE);

            int32_t t = g_latest_weather.temperature_x100;
            sprintf(buf, "Temp: %ld.%02ld C       ", t / 100, (t < 0 ? -t : t) % 100);
            vLcdDrawTextSafe(buf, 8, BLACK, 2, WHITE);
            sprintf(buf, "Humi: %lu.%02lu %%        ", g_latest_weather.humidity_x100 / 100, g_latest_weather.humidity_x100 % 100);
            vLcdDrawTextSafe(buf, 10, BLACK, 2, WHITE);
            sprintf(buf, "Pres: %lu.%02lu hPa ", g_latest_weather.pressure_pa / 100, g_latest_weather.pressure_pa % 100);
            vLcdDrawTextSafe(buf, 12, BLACK, 2, WHITE);
            vLcdDrawTextSafe("                     ", 14, WHITE, 2, WHITE);
        } 
        else if (!g_can_status_ok) 
        {
            vLcdDrawTextSafe(" !! CAN ERROR !!    ", 8, WHITE, 2, RED);
            vLcdDrawTextSafe(" NO DATA RECEIVED   ", 10, WHITE, 2, RED);
        }
        else 
        {
            vLcdDrawTextSafe(" !! SENSOR ERROR !! ", 12, WHITE, 2, RED);
            vLcdDrawTextSafe("  PLEASE RECONNECT  ", 14, WHITE, 2, RED);
        }
        sprintf(buf, "Update Event: %lu ", g_heartbeat);
        vLcdDrawTextSafe(buf, 15, BLUE, 1, WHITE);
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
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
