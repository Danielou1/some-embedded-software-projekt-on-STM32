/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V3.6 (Industrial Documentation Edition)
 * @brief   Weather Station - High-Reliability Embedded System.
 * 
 * @details This application implements a real-time weather monitoring system
 *          using the STM32F429 microcontroller and FreeRTOS.
 *          
 *          Architecture Overview:
 *          - Data Acquisition: BME280 sensor (I2C1 bus).
 *          - Data Transmission: CAN1 bus (Loopback mode for self-test).
 *          - User Interface: LCD (parallel/SPI interface).
 *          
 *          Concurrency Control & Sync:
 *          - Mutex: Protects the shared LCD resource from race conditions.
 *          - Binary Semaphore: Synchronizes the UI refresh with hardware events
 *            (CAN reception or sensor failure).
 *          - Message Queue: Buffers data between the sensor producer and CAN consumer.
 *
 *          Data Integrity:
 *          - Uses fixed-point arithmetic (int32_t x100) to avoid floating-point
 *            unit (FPU) overhead and stack overflow issues in RTOS tasks.
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

/**
 * @brief Structure to store environment data using fixed-point representation.
 * temperature and humidity are multiplied by 100 to preserve 2 decimal places.
 */
typedef struct {
    int32_t temperature_x100;  /**< Temp in C * 100 */
    uint32_t pressure_pa;      /**< Pressure in Pascals */
    uint32_t humidity_x100;    /**< Humidity in % * 100 */
} WeatherData_t;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void vSensorTask(void *pvParameters);
void vCanTask(void *pvParameters);
void vUiTask(void *pvParameters);
void vLcdDrawTextSafe(char *text, uint8_t line, uint16_t color, uint8_t size, uint16_t bgcolor);

/* Global handles (RTOS Objects) ---------------------------------------------*/
TaskHandle_t xSensorTaskHandle = NULL; /**< Handle for acquisition task */
TaskHandle_t xCanTaskHandle    = NULL; /**< Handle for communication task */
TaskHandle_t xUiTaskHandle     = NULL; /**< Handle for display task */

QueueHandle_t xWeatherQueue    = NULL; /**< Inter-task data buffer */
SemaphoreHandle_t xLcdMutex    = NULL; /**< Resource lock for LCD */
SemaphoreHandle_t xDataReadySync = NULL; /**< Event flag for UI refresh */

/* Shared Global Data (Volatile for thread-safety) ---------------------------*/
volatile WeatherData_t g_latest_weather = {0}; /**< Latest valid measurements */
volatile uint32_t g_heartbeat = 0;             /**< Counter for update events */
volatile uint32_t g_last_can_rx_tick = 0;      /**< Timestamp of last successful CAN RX */
volatile bool g_sensor_data_valid = false;     /**< Real-time status of I2C sensor */
volatile bool g_can_status_ok = false;         /**< Real-time status of CAN bus */

/**
 * @brief Application Entry Point.
 * Initializes HAL, system clocks, and peripherals before starting the RTOS scheduler.
 */
int main(void)
{
    /* 1. Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();
    
    /* 2. Configure the system clock to 168 MHz */
    SystemClock_Config();

    /* 3. Initialize User Interface (LCD) */
    lcd_init();
    lcd_fill_screen(WHITE);
    lcd_draw_text_at_line("P2: Reactive RTOS", 1, BLACK, 2, WHITE);

    /* 4. Probe Hardware: BME280 Environment Sensor (I2C) */
    if (env_sensor_init() == BME280_OK) {
        lcd_draw_text_at_line("BME280: OK", 3, GREEN, 2, WHITE);
    } else {
        lcd_draw_text_at_line("BME280: OFF", 3, RED, 2, WHITE);
    }

    /* 5. Initialize Modular CAN Driver (CAN1) */
    if (can_com_init() == 0) {
        lcd_draw_text_at_line("CAN Bus: OK", 4, GREEN, 2, WHITE);
    } else {
        lcd_draw_text_at_line("CAN Bus: OFF", 4, RED, 2, WHITE);
    }

    /* 6. Initialize RTOS Sync/Communication Objects */
    xLcdMutex = xSemaphoreCreateMutex();
    xDataReadySync = xSemaphoreCreateBinary();
    xWeatherQueue = xQueueCreate(5, sizeof(WeatherData_t));

    if (xLcdMutex != NULL && xWeatherQueue != NULL)
    {
        /* 7. Create System Tasks */
        xTaskCreate(vSensorTask, "Sensor", 512, NULL, 2, &xSensorTaskHandle);
        xTaskCreate(vCanTask,    "CAN",    512, NULL, 3, &xCanTaskHandle);
        xTaskCreate(vUiTask,     "UI",     512, NULL, 1, &xUiTaskHandle);

        /* 8. Start Scheduler */
        vTaskStartScheduler();
    }

    /* Infinite loop if scheduler fails */
    while (1);
}

/**
 * @brief CAN RX Interrupt Service Routine Callback.
 * @note  Triggered when a message is pending in FIFO0.
 * Decodes the CAN frame and signals the UI task via Semaphore.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    /* Extract message from the hardware FIFO */
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
    {
        /* Filter by Standard ID 0x400 (Weather Data) */
        if (rxHeader.StdId == 0x400)
        {
            /* Data Unpacking (Big-Endian format) */
            int16_t temp = (int16_t)((rxData[0] << 8) | rxData[1]);
            uint16_t hum = (uint16_t)((rxData[2] << 8) | rxData[3]);
            uint32_t press = (uint32_t)((rxData[4] << 24) | (rxData[5] << 16) | (rxData[6] << 8) | rxData[7]);

            /* Atomic update of shared variables */
            g_latest_weather.temperature_x100 = temp;
            g_latest_weather.humidity_x100 = hum;
            g_latest_weather.pressure_pa = press * 10;

            g_last_can_rx_tick = xTaskGetTickCountFromISR();
            g_can_status_ok = true;

            /* Context Switch: Signal UI task to refresh immediately */
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xSemaphoreGiveFromISR(xDataReadySync, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

/**
 * @brief CAN Error Callback.
 * Handles bus failures, mailbox overflows, and disconnection events.
 */
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
    g_can_status_ok = false;
    
    /* Force UI refresh to display error status */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xDataReadySync, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Thread-safe LCD output function.
 * Uses a Mutex to prevent multiple tasks from writing to the LCD controller simultaneously.
 * @param text The string to display.
 * @param line The LCD line number.
 * @param color Text color.
 * @param size Font size.
 * @param bgcolor Background color.
 */
void vLcdDrawTextSafe(char *text, uint8_t line, uint16_t color, uint8_t size, uint16_t bgcolor)
{
    if (xLcdMutex == NULL) return;
    
    /* Wait up to 50ms for the resource to be free */
    if (xSemaphoreTake(xLcdMutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        lcd_draw_text_at_line(text, line, color, size, bgcolor);
        
        /* Always release the mutex after writing */
        xSemaphoreGive(xLcdMutex);
    }
}

/**
 * @brief Task: Sensor Data Acquisition (PRODUCER).
 * Periodically reads the BME280 sensor and pushes data to the CAN transmission queue.
 */
void vSensorTask(void *pvParameters)
{
    struct bme280_data sensor_raw;
    WeatherData_t data;

    for (;;) {
        /* I2C Transaction: Blocking call */
        if (env_sensor_read_data(&sensor_raw) == BME280_OK) {
            /* Convert floating point to fixed-point */
            data.temperature_x100 = (int32_t)(sensor_raw.temperature * 100);
            data.pressure_pa      = (uint32_t)(sensor_raw.pressure);
            data.humidity_x100    = (uint32_t)(sensor_raw.humidity * 100);
            
            g_sensor_data_valid = true;
            
            /* Push to queue (non-blocking) */
            xQueueSend(xWeatherQueue, &data, 0); 
        } else {
            /* Handle sensor disconnection */
            g_sensor_data_valid = false;
            xSemaphoreGive(xDataReadySync); /* Notify UI of failure */
        }
        
        /* Wait 1 second (Task enters Blocked state) */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Task: CAN Communication (CONSUMER).
 * Monitors the queue for new weather data and transmits it over the CAN bus.
 */
void vCanTask(void *pvParameters)
{
    WeatherData_t received;
    CAN_TxHeaderTypeDef txHeader = { .StdId = 0x400, .RTR = CAN_RTR_DATA, .IDE = CAN_ID_STD, .DLC = 8 };
    uint8_t txData[8];
    uint32_t txMailbox;

    for (;;) {
        /* Wait indefinitely for data from the Queue */
        if (xQueueReceive(xWeatherQueue, &received, portMAX_DELAY) == pdPASS) {
            /* Fixed-point to bytes conversion */
            int16_t temp = (int16_t)(received.temperature_x100);
            uint16_t hum = (uint16_t)(received.humidity_x100);
            uint32_t press = received.pressure_pa / 10;

            /* Fill 8-byte CAN payload */
            txData[0] = (temp >> 8) & 0xFF; txData[1] = temp & 0xFF;
            txData[2] = (hum >> 8) & 0xFF;  txData[3] = hum & 0xFF;
            txData[4] = (press >> 24) & 0xFF; txData[5] = (press >> 16) & 0xFF;
            txData[6] = (press >> 8) & 0xFF;  txData[7] = press & 0xFF;
            
            /* HAL CAN Transmit (Asynchronous) */
            HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox);
        }
    }
}

/**
 * @brief Task: User Interface (EVENT-DRIVEN CONSUMER).
 * This task is suspended until a hardware event occur (CAN RX or error).
 */
void vUiTask(void *pvParameters)
{
    char buf[32];
    for (;;) {
        /* Suspend task until signalled by an interrupt or another task */
        xSemaphoreTake(xDataReadySync, portMAX_DELAY);
        
        g_heartbeat++;

        /* CAN Watchdog: Check if data is too old (Silent failure detection) */
        if ((xTaskGetTickCount() - g_last_can_rx_tick) > pdMS_TO_TICKS(2500)) {
            g_can_status_ok = false;
        }

        /* Update Device Status Labels (Lines 3 & 4) */
        vLcdDrawTextSafe(g_sensor_data_valid ? "BME280: OK  " : "BME280: OFF ", 3, g_sensor_data_valid ? GREEN : RED, 2, WHITE);
        vLcdDrawTextSafe(g_can_status_ok ? "CAN Bus: OK " : "CAN Bus: OFF", 4, g_can_status_ok ? GREEN : RED, 2, WHITE);

        /* Update Environmental Data Display */
        if (g_can_status_ok && g_sensor_data_valid) 
        {
            /* Normal Mode: Clean up previous error zones */
            vLcdDrawTextSafe("                     ", 14, WHITE, 2, WHITE);

            int32_t t = g_latest_weather.temperature_x100;
            sprintf(buf, "Temp: %ld.%02ld C       ", t / 100, (t < 0 ? -t : t) % 100);
            vLcdDrawTextSafe(buf, 8, BLACK, 2, WHITE);

            sprintf(buf, "Humi: %lu.%02lu %%        ", g_latest_weather.humidity_x100 / 100, g_latest_weather.humidity_x100 % 100);
            vLcdDrawTextSafe(buf, 10, BLACK, 2, WHITE);

            sprintf(buf, "Pres: %lu.%02lu hPa     ", g_latest_weather.pressure_pa / 100, g_latest_weather.pressure_pa % 100);
            vLcdDrawTextSafe(buf, 12, BLACK, 2, WHITE);
        } 
        else if (!g_can_status_ok) 
        {
            /* CAN Failure display */
            vLcdDrawTextSafe(" !! CAN ERROR !!    ", 8, WHITE, 2, RED);
            vLcdDrawTextSafe(" NO DATA RECEIVED   ", 10, WHITE, 2, RED);
        }
        else 
        {
            /* Sensor Failure display */
            vLcdDrawTextSafe(" !! SENSOR ERROR !! ", 12, WHITE, 2, RED);
            vLcdDrawTextSafe("  PLEASE RECONNECT  ", 14, WHITE, 2, RED);
        }
        
        /* System Health Monitor */
        sprintf(buf, "Update Event: %lu ", g_heartbeat);
        vLcdDrawTextSafe(buf, 15, BLUE, 1, WHITE);
    }
}

/**
 * @brief System Clock Configuration (168 MHz).
 * Configures the HSE (External High Speed) oscillator and PLL.
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

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}