/**
 ******************************************************************************
 * @file    joystick.h
 * @author  Danielou Mounsande
 * @version V2.1
 * @date    12-November-2025
 * @brief   Header file for the joystick driver module
 *          for the Waveshare board's joystick.
 ******************************************************************************
 */

#ifndef JOYSTICK_JOYSTICK_H_
#define JOYSTICK_JOYSTICK_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Public Data Types ---------------------------------------------------------*/

/**
 * @brief Configuration structure for the joystick.
 * @note  This structure allows defining the GPIO pins to which each
 *        direction and the select button of the joystick are connected.
 */
typedef struct {
    GPIO_TypeDef* port;     /*!< The GPIO port to which the joystick is connected (e.g., GPIOG). */
    uint16_t pin_right;     /*!< Pin for the Right direction. */
    uint16_t pin_down;      /*!< Pin for the Down direction. */
    uint16_t pin_left;      /*!< Pin for the Left direction. */
    uint16_t pin_up;        /*!< Pin for the Up direction. */
    uint16_t pin_select;    /*!< Pin for the Select (center) button. */
} Joystick_Config_t;

/* Public Function Prototypes ------------------------------------------------*/

/**
 * @brief Initializes the joystick module with a specific pin configuration.
 * @param config Pointer to the pin configuration structure.
 * @note  This function must be called before any other function in the module.
 *        It handles the initialization of the GPIO pins.
 */
void joystick_init(Joystick_Config_t* config);

/**
 * @brief Reads the state of the joystick's input pins.
 * @retval uint16_t: A bitmask where each bit corresponds to a joystick pin.
 *          The bit is set to '1' if the associated button is pressed.
 * @note   The joystick buttons are "low-active". The result is inverted
 *         to provide a "high-active" mask. Use the pins defined in the
 *         Joystick_Config_t struct to check the result (e.g., `if (result & config.pin_up)`).
 */
uint16_t joystick_read(void);

#endif /* JOYSTICK_JOYSTICK_H_ */
