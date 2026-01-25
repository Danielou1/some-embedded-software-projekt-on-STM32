/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V2.5
 * @brief   Weather Station with FreeRTOS - Full Version (Integer Only).
 *
 * @note    This version implements a robust multitasking architecture using 
 *          fixed-point arithmetic to avoid stack overflows associated with 
 *          floating-point formatting in RTOS tasks.
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
/**
 * @brief Structure to hold environmental data in fixed-point format.
 */
typedef struct {
    int32_t temperature_x100;
    uint32_t pressure_pa;      /* Pressure in Pascals */
    uint32_t humidity_x100;
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

/* Shared data (Accessed by UI task) */
volatile WeatherData_t g_latest_weather = {0};
volatile uint32_t g_heartbeat = 0;
bool g_bme_present = false;

/**
  * @brief  The application's main entry point.
  * @retval int
  */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	HAL_Init();
    
    /* IMPORTANT for FreeRTOS: Set Priority Group to 4 */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    SystemClock_Config();

	/* Initialize Peripherals -------------------------------------------------*/
	lcd_init();
	lcd_fill_screen(WHITE);
	lcd_draw_text_at_line("P2: RTOS W-Station", 1, BLACK, 2, WHITE);

	/* Initialize BME280 Sensor */
	if (env_sensor_init() == BME280_OK) {
        g_bme_present = true;
		lcd_draw_text_at_line("BME280: OK", 3, GREEN, 2, WHITE);
	} else {
        g_bme_present = false;
		lcd_draw_text_at_line("BME280: NOT FOUND", 3, RED, 2, WHITE);
	}

    /* Initialize CAN peripheral */
    if (can_com_init() == 0) {
		lcd_draw_text_at_line("CAN: Initialized", 4, GREEN, 2, WHITE);
	} else {
		lcd_draw_text_at_line("CAN: Init Error", 4, RED, 2, WHITE);
	}

    /* FreeRTOS Objects Creation ----------------------------------------------*/

    /* Create a Queue for 5 weather data samples */
    xWeatherQueue = xQueueCreate(5, sizeof(WeatherData_t));

    if (xWeatherQueue != NULL)
    {
        /* Tasks Creation - Priority levels: CAN(3) > Sensor(2) > UI(1) */
        xTaskCreate(vSensorTask, "Sensor", 512, NULL, 2, &xSensorTaskHandle);
        xTaskCreate(vCanTask,    "CAN",    512, NULL, 3, &xCanTaskHandle);
        xTaskCreate(vUiTask,     "UI",     512, NULL, 1, &xUiTaskHandle);

        lcd_draw_text_at_line("OS Starting...", 15, BLUE, 1, WHITE);
        
        /* Start the Scheduler */
        vTaskStartScheduler();
    }

	/* Should never reach here unless there's a heap memory issue */
    lcd_fill_screen(RED);
    lcd_draw_text_at_line("RTOS START FAILED!", 8, WHITE, 2, RED);
	while (1);
}

/**
 * @brief Task responsible for reading sensor data.
 */
void vSensorTask(void *pvParameters)
{
    struct bme280_data sensor_raw;
    WeatherData_t data;

    for (;;)
    {
        if (g_bme_present) 
        {
            if (env_sensor_read_data(&sensor_raw) == BME280_OK)
            {
                /* Safe conversion from float to fixed-point integer (x100) */
                data.temperature_x100 = (int32_t)(sensor_raw.temperature * 100);
                data.pressure_pa      = (uint32_t)(sensor_raw.pressure);
                data.humidity_x100    = (uint32_t)(sensor_raw.humidity * 100);
                
                g_latest_weather = data;
                /* Send data to CAN task without blocking if queue is full */
                xQueueSend(xWeatherQueue, &data, 0); 
            }
        }
        /* Wait for 1 second */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Task responsible for transmitting data via CAN bus.
 */
void vCanTask(void *pvParameters)
{
    WeatherData_t received;
    CAN_TxHeaderTypeDef txHeader;
    uint8_t txData[8];
    uint32_t txMailbox;

    txHeader.StdId = 0x400; /* Weather Data ID */
    txHeader.RTR   = CAN_RTR_DATA;
    txHeader.IDE   = CAN_ID_STD;
    txHeader.DLC   = 8; /* Sending 8 bytes */
    txHeader.TransmitGlobalTime = DISABLE;

    for (;;)
    {
        /* Block indefinitely until data is available in the queue */
        if (xQueueReceive(xWeatherQueue, &received, portMAX_DELAY) == pdPASS)
        {
            /* Pack Temp, Hum, and Press into CAN message */
            int16_t temp = (int16_t)(received.temperature_x100);
            uint16_t hum = (uint16_t)(received.humidity_x100);
            uint32_t press = received.pressure_pa / 10; /* Compress for example */

            txData[0] = (temp >> 8) & 0xFF;
            txData[1] = temp & 0xFF;
            txData[2] = (hum >> 8) & 0xFF;
            txData[3] = hum & 0xFF;
            txData[4] = (press >> 24) & 0xFF;
            txData[5] = (press >> 16) & 0xFF;
            txData[6] = (press >> 8) & 0xFF;
            txData[7] = press & 0xFF;
            
            /* HAL_CAN_AddTxMessage is thread-safe in this context */
            HAL_CAN_AddTxMessage(&hcan1, &txHeader, txData, &txMailbox);
        }
    }
}

/**
 * @brief Task responsible for updating the LCD display.
 */
void vUiTask(void *pvParameters)
{
    char buf[32];
    for (;;)
    {
        g_heartbeat++;
        
        if (g_bme_present) {
            /* Display Temperature using fixed-point formatting */
            int32_t t = g_latest_weather.temperature_x100;
            sprintf(buf, "Temp: %ld.%02ld C", t / 100, (t < 0 ? -t : t) % 100);
            lcd_draw_text_at_line(buf, 8, BLACK, 2, WHITE);

            /* Display Humidity */
            uint32_t h = g_latest_weather.humidity_x100;
            sprintf(buf, "Humi: %lu.%02lu %%", h / 100, h % 100);
            lcd_draw_text_at_line(buf, 10, BLACK, 2, WHITE);

            /* Display Pressure (Convert Pa to hPa) */
            uint32_t p = g_latest_weather.pressure_pa;
            sprintf(buf, "Pres: %lu.%02lu hPa", p / 100, p % 100);
            lcd_draw_text_at_line(buf, 12, BLACK, 2, WHITE);
        } else {
            lcd_draw_text_at_line("No Sensor Connected", 8, RED, 1, WHITE);
        }

        /* System Heartbeat / Health Monitor */
        sprintf(buf, "RTOS Ticks: %lu", g_heartbeat);
        lcd_draw_text_at_line(buf, 14, GREEN, 1, WHITE);

        /* Update UI at 4Hz */
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

/**
 * @brief  System Clock Configuration (168 MHz).
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
