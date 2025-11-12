/**
 ******************************************************************************
 * @file    potis.c
 * @author  Danielou Mounsande
 * @version V2.0
 * @date    12-November-2025
 * @brief   Implementation file for the potentiometer ADC reader module.
 ******************************************************************************
 */

#include "potis.h"
#include "stm32f4xx_hal_dma.h"
#include <string.h> // For memset

/* Private Defines -----------------------------------------------------------*/
#define NUM_POTIS           2U      /*!< The number of potentiometers being read. */
#define SAMPLES_PER_POTI    100U    /*!< The number of samples to average for each potentiometer in DMA mode. */
#define ADC_BUF_LEN         (NUM_POTIS * SAMPLES_PER_POTI) /*!< Total length of the DMA buffer. */

/* Private Variables ---------------------------------------------------------*/

/** @brief ADC handle for ADC1 peripheral. */
static ADC_HandleTypeDef hadc1;

/** @brief DMA handle for DMA2 Stream 0, used by ADC1. */
static DMA_HandleTypeDef hdma_adc1;

/** @brief Buffer to store ADC conversion results from DMA.
 *  @note The buffer is interleaved: [Poti1_Sample1, Poti2_Sample1, Poti1_Sample2, Poti2_Sample2, ...].
 */
static uint16_t adc_dma_buffer[ADC_BUF_LEN];

/** @brief Stores the current operating mode (Polling or DMA) of the module. */
static PotiMode current_poti_mode;

/* HAL MSP Functions ---------------------------------------------------------*/

/**
 * @brief ADC MSP Initialization.
 *        This function is called by HAL_ADC_Init() to configure the low-level
 *        hardware resources used by the ADC peripheral.
 * @param hadc Pointer to the ADC_HandleTypeDef structure.
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hadc->Instance == ADC1) {
        /* 1. Enable peripherals and GPIO clocks */
        __HAL_RCC_ADC1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* 2. Configure GPIO pins for analog input */
        // PA6 is connected to ADC1_IN6 (Poti 1)
        // PA7 is connected to ADC1_IN7 (Poti 2)
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* 3. Configure DMA if in DMA mode */
        if (current_poti_mode == POTI_MODE_DMA) {
            __HAL_RCC_DMA2_CLK_ENABLE();

            // ADC1 uses DMA2, Stream 0, Channel 0
            hdma_adc1.Instance = DMA2_Stream0;
            hdma_adc1.Init.Channel = DMA_CHANNEL_0;
            hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
            hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
            hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
            hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 16-bit for 12-bit ADC
            hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
            hdma_adc1.Init.Mode = DMA_CIRCULAR; // Continuous conversion
            hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
            hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            HAL_DMA_Init(&hdma_adc1);

            /* 4. Link DMA to ADC */
            __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc1);
        }
    }
}

/**
 * @brief ADC MSP De-Initialization.
 *        This function is called by HAL_ADC_DeInit() to de-initialize the
 *        low-level hardware resources.
 * @param hadc Pointer to the ADC_HandleTypeDef structure.
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        /* 1. Disable peripherals clock */
        __HAL_RCC_ADC1_CLK_DISABLE();

        /* 2. De-initialize GPIOA pins */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6 | GPIO_PIN_7);

        /* 3. De-initialize DMA if it was used */
        if (current_poti_mode == POTI_MODE_DMA) {
            HAL_DMA_DeInit(&hdma_adc1);
        }
    }
}


/* Public Functions ----------------------------------------------------------*/

void potis_init(PotiMode mode) {
    ADC_ChannelConfTypeDef sConfig = {0};
    current_poti_mode = mode;

    // Configure the global ADC parameters
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV6; // ADC Clock = APB2_CLK / 6
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;

    if (mode == POTI_MODE_POLLING) {
        hadc1.Init.ScanConvMode = DISABLE;
        hadc1.Init.ContinuousConvMode = DISABLE;
        hadc1.Init.NbrOfConversion = 1;
        hadc1.Init.DMAContinuousRequests = DISABLE;
    } else { // POTI_MODE_DMA
        hadc1.Init.ScanConvMode = ENABLE;
        hadc1.Init.ContinuousConvMode = ENABLE;
        hadc1.Init.NbrOfConversion = NUM_POTIS;
        hadc1.Init.DMAContinuousRequests = ENABLE;
    }

    // This call will trigger HAL_ADC_MspInit()
    HAL_ADC_Init(&hadc1);

    // Configure ADC channels only if in DMA mode, as polling mode configures them on-the-fly.
    if (mode == POTI_MODE_DMA) {
        // Configure Channel 6 for Poti 1
        sConfig.Channel = ADC_CHANNEL_6;
        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        // Configure Channel 7 for Poti 2
        sConfig.Channel = ADC_CHANNEL_7;
        sConfig.Rank = 2;
        sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        // Start ADC conversion in DMA mode and clear the buffer
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, ADC_BUF_LEN);
        memset(adc_dma_buffer, 0, sizeof(adc_dma_buffer));
    }
}

uint32_t potis_get_val(uint8_t poti_num) {
    uint32_t adc_value = 0;

    if (current_poti_mode == POTI_MODE_POLLING) {
        ADC_ChannelConfTypeDef sConfig = {0};

        // Dynamically configure the channel for this specific reading
        if (poti_num == POTI_1) {
            sConfig.Channel = ADC_CHANNEL_6;
        } else if (poti_num == POTI_2) {
            sConfig.Channel = ADC_CHANNEL_7;
        } else {
            return 0; // Invalid potentiometer number
        }

        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);

        // Perform a single conversion
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
            adc_value = HAL_ADC_GetValue(&hadc1);
        }
        HAL_ADC_Stop(&hadc1);

    } else { // POTI_MODE_DMA
        uint32_t sum = 0;
        uint32_t start_index;

        if (poti_num == POTI_1) {
            start_index = 0; // Poti 1 samples are at even indices (0, 2, 4, ...)
        } else if (poti_num == POTI_2) {
            start_index = 1; // Poti 2 samples are at odd indices (1, 3, 5, ...)
        } else {
            return 0; // Invalid potentiometer number
        }

        // Average the collected samples from the DMA buffer to get a stable value
        for (uint32_t i = 0; i < SAMPLES_PER_POTI; i++) {
            sum += adc_dma_buffer[start_index + (i * NUM_POTIS)];
        }
        adc_value = sum / SAMPLES_PER_POTI;
    }

    return adc_value;
}