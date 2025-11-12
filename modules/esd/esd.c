/**
 ******************************************************************************
 * @file    esd.c
 * @author  Danielou Mounsande
 * @version V2.1
 * @date    12-November-2025
 * @brief   Implementation file for the 7-segment display driver module
 *          (ESD - 8-Segment LED Board).
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "esd/esd.h"

/* Private Variables ---------------------------------------------------------*/

// Local copy of the pin configuration provided by the user.
static ESD_Config_t g_esd_config;

/**
 * @brief Mapping table for digits (0-9) to the 7 segments.
 * @note  The logic is for a **common anode** display.
 *        A `0` turns the segment ON (pin state LOW).
 *        A `1` turns the segment OFF (pin state HIGH).
 *        Segment order: {A, B, C, D, E, F, G}
 */
static const uint8_t digit_map[10][7] = {
    {0, 0, 0, 0, 0, 0, 1}, // 0
    {1, 0, 0, 1, 1, 1, 1}, // 1
    {0, 0, 1, 0, 0, 1, 0}, // 2
    {0, 0, 0, 0, 1, 1, 0}, // 3
    {1, 0, 0, 1, 1, 0, 0}, // 4
    {0, 1, 0, 0, 1, 0, 0}, // 5
    {0, 1, 0, 0, 0, 0, 0}, // 6
    {0, 0, 0, 1, 1, 1, 1}, // 7
    {0, 0, 0, 0, 0, 0, 0}, // 8
    {0, 0, 0, 0, 1, 0, 0}  // 9
};

/* Private function prototypes -----------------------------------------------*/
static void ESD_GPIO_Init(void);


/* Public function implementations -------------------------------------------*/

/**
 * @brief Initializes the ESD module.
 * @param config Pointer to the pin configuration structure.
 * @note  This function copies the pin configuration and initializes the
 *        necessary GPIOs. It must be called once at startup.
 */
void esd_init(ESD_Config_t* config) {
    // 1. Copy the pin configuration locally
    g_esd_config = *config;

    // 2. Initialize the GPIO pins
    ESD_GPIO_Init();
}

/**
 * @brief Displays a digit at a given position on the display.
 * @param digit The digit to display (from ESD_DIGIT_0 to ESD_DIGIT_9).
 * @param pos The position where to display the digit (from ESD_POSITION_1 to ESD_POSITION_4).
 * @note  For multiplexing, this function must be called rapidly and
 *        successively for each position to give the illusion that
 *        all digits are lit simultaneously.
 */
void esd_show_digit(esd_digit_t digit, esd_position_t pos) {
    // Turn off all segments and all positions to prevent "ghosting"
    HAL_GPIO_WritePin(g_esd_config.led_a.port, g_esd_config.led_a.pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(g_esd_config.led_b.port, g_esd_config.led_b.pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(g_esd_config.led_c.port, g_esd_config.led_c.pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(g_esd_config.led_d.port, g_esd_config.led_d.pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(g_esd_config.led_e.port, g_esd_config.led_e.pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(g_esd_config.led_f.port, g_esd_config.led_f.pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(g_esd_config.led_g.port, g_esd_config.led_g.pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(g_esd_config.cntl1.port, g_esd_config.cntl1.pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(g_esd_config.cntl2.port, g_esd_config.cntl2.pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(g_esd_config.cntl3.port, g_esd_config.cntl3.pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(g_esd_config.cntl4.port, g_esd_config.cntl4.pin, GPIO_PIN_RESET);

    // Light up the segments corresponding to the requested digit
    if (digit >= ESD_DIGIT_0 && digit <= ESD_DIGIT_9) {
        if (!digit_map[digit][0]) HAL_GPIO_WritePin(g_esd_config.led_a.port, g_esd_config.led_a.pin, GPIO_PIN_RESET);
        if (!digit_map[digit][1]) HAL_GPIO_WritePin(g_esd_config.led_b.port, g_esd_config.led_b.pin, GPIO_PIN_RESET);
        if (!digit_map[digit][2]) HAL_GPIO_WritePin(g_esd_config.led_c.port, g_esd_config.led_c.pin, GPIO_PIN_RESET);
        if (!digit_map[digit][3]) HAL_GPIO_WritePin(g_esd_config.led_d.port, g_esd_config.led_d.pin, GPIO_PIN_RESET);
        if (!digit_map[digit][4]) HAL_GPIO_WritePin(g_esd_config.led_e.port, g_esd_config.led_e.pin, GPIO_PIN_RESET);
        if (!digit_map[digit][5]) HAL_GPIO_WritePin(g_esd_config.led_f.port, g_esd_config.led_f.pin, GPIO_PIN_RESET);
        if (!digit_map[digit][6]) HAL_GPIO_WritePin(g_esd_config.led_g.port, g_esd_config.led_g.pin, GPIO_PIN_RESET);
    }

    // Activate the correct position (the correct digit)
    switch (pos) {
        case ESD_POSITION_1:
            HAL_GPIO_WritePin(g_esd_config.cntl1.port, g_esd_config.cntl1.pin, GPIO_PIN_SET);
            break;
        case ESD_POSITION_2:
            HAL_GPIO_WritePin(g_esd_config.cntl2.port, g_esd_config.cntl2.pin, GPIO_PIN_SET);
            break;
        case ESD_POSITION_3:
            HAL_GPIO_WritePin(g_esd_config.cntl3.port, g_esd_config.cntl3.pin, GPIO_PIN_SET);
            break;
        case ESD_POSITION_4:
            HAL_GPIO_WritePin(g_esd_config.cntl4.port, g_esd_config.cntl4.pin, GPIO_PIN_SET);
            break;
        case ESD_POSITION_ALL:
            HAL_GPIO_WritePin(g_esd_config.cntl1.port, g_esd_config.cntl1.pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(g_esd_config.cntl2.port, g_esd_config.cntl2.pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(g_esd_config.cntl3.port, g_esd_config.cntl3.pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(g_esd_config.cntl4.port, g_esd_config.cntl4.pin, GPIO_PIN_SET);
            break;
    }
}

/* Private function implementations ------------------------------------------*/

/**
 * @brief Initializes the GPIO pins required for the 7-segment display.
 * @note  This function is called by esd_init(). It is based on the
 *        configuration stored in g_esd_config.
 */
static void ESD_GPIO_Init(void) {
    // Enable clocks for GPIO ports D and E
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

    // Initialize pins on port D
    gpio_init.Pin = g_esd_config.cntl1.pin | g_esd_config.cntl2.pin | g_esd_config.cntl3.pin | g_esd_config.cntl4.pin |
                    g_esd_config.led_a.pin | g_esd_config.led_b.pin | g_esd_config.led_c.pin | g_esd_config.led_d.pin |
                    g_esd_config.led_e.pin | g_esd_config.led_f.pin;
    HAL_GPIO_Init(GPIOD, &gpio_init);

    // Initialize pins on port E
    gpio_init.Pin = g_esd_config.led_g.pin | g_esd_config.point.pin | g_esd_config.dot.pin;
    HAL_GPIO_Init(GPIOE, &gpio_init);
}
