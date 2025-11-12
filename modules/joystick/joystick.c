/**
 ******************************************************************************
 * @file    joystick.c
 * @author  Danielou Mounsande
 * @version V2.1
 * @date    12-November-2025
 * @brief   Implementation file for the joystick driver module
 *          for the Waveshare board's joystick.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "joystick/joystick.h"

/* Private Variables ---------------------------------------------------------*/

/** @brief Local copy of the joystick's pin configuration. */
static Joystick_Config_t g_joystick_config;

/* Private function prototypes -----------------------------------------------*/
static void Joystick_GPIO_Init(void);

/* Public function implementations -------------------------------------------*/

/**
 * @brief  Initializes the joystick module by storing the pin configuration
 *         and configuring the corresponding GPIOs.
 * @param  config: A pointer to the Joystick_Config_t struct with the pinout.
 * @retval None
 */
void joystick_init(Joystick_Config_t* config) {
    // 1. Store the pin configuration locally
    g_joystick_config = *config;

    // 2. Initialize the joystick's GPIO pins
    Joystick_GPIO_Init();
}

/**
 * @brief  Reads the state of the joystick's input pins.
 * @param  None
 * @retval uint16_t: A bitmask where one or more bits defined in the config
 *         struct are set to '1' if the associated button is pressed.
 * @note   The joystick buttons are "low-active". The result is inverted
 *         by the '~' operator to get a "high-active" result.
 */
uint16_t joystick_read(void) {
    uint16_t pins_all = g_joystick_config.pin_right | g_joystick_config.pin_down |
                        g_joystick_config.pin_left | g_joystick_config.pin_up |
                        g_joystick_config.pin_select;

    // Invert the state of the input data register (IDR) to get a high-active result
    return pins_all & (~g_joystick_config.port->IDR);
}

/* Private function implementations ------------------------------------------*/

/**
 * @brief  Configures the GPIO pins for the joystick port.
 * @note   This function is called by joystick_init(). It is based on the
 *         configuration stored in g_joystick_config.
 * @retval None
 */
static void Joystick_GPIO_Init(void) {
    // Enable the clock for the GPIO port used by the joystick
    __HAL_RCC_GPIOG_CLK_ENABLE(); // The joystick is on port G

    GPIO_InitTypeDef gpio_init_joy = { 0 };
    gpio_init_joy.Pin = g_joystick_config.pin_right | g_joystick_config.pin_down |
                        g_joystick_config.pin_left | g_joystick_config.pin_up |
                        g_joystick_config.pin_select;
    gpio_init_joy.Mode = GPIO_MODE_INPUT;   // Pins configured as input
    gpio_init_joy.Pull = GPIO_PULLUP;       // Pull-up resistors enabled (for low-active buttons)

    HAL_GPIO_Init(g_joystick_config.port, &gpio_init_joy);
}
