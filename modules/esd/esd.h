/**
 ******************************************************************************
 * @file    esd.h
 * @author  Danielou Mounsande
 * @version V2.1
 * @date    12-November-2025
 * @brief   Header file for the 7-segment display driver module
 *          (ESD - 8-Segment LED Board).
 ******************************************************************************
 */

#ifndef ESD_ESD_H_
#define ESD_ESD_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Public Data Types ---------------------------------------------------------*/

/**
 * @brief Structure to define a GPIO pin (port and pin number).
 */
typedef struct {
    GPIO_TypeDef* port; /*!< Pointer to the GPIO port (e.g., GPIOD). */
    uint16_t pin;       /*!< The pin number (e.g., GPIO_PIN_14). */
} ESD_Pin_t;

/**
 * @brief Complete configuration structure for the ESD display.
 * @note  The user must fill this structure with the pins corresponding
 *        to their wiring and pass it to the esd_init() function.
 */
typedef struct {
    ESD_Pin_t cntl1;    /*!< Control pin for digit 1. */
    ESD_Pin_t cntl2;    /*!< Control pin for digit 2. */
    ESD_Pin_t cntl3;    /*!< Control pin for digit 3. */
    ESD_Pin_t cntl4;    /*!< Control pin for digit 4. */
    ESD_Pin_t led_a;    /*!< Pin for segment 'a'. */
    ESD_Pin_t led_b;    /*!< Pin for segment 'b'. */
    ESD_Pin_t led_c;    /*!< Pin for segment 'c'. */
    ESD_Pin_t led_d;    /*!< Pin for segment 'd'. */
    ESD_Pin_t led_e;    /*!< Pin for segment 'e'. */
    ESD_Pin_t led_f;    /*!< Pin for segment 'f'. */
    ESD_Pin_t led_g;    /*!< Pin for segment 'g'. */
    ESD_Pin_t point;    /*!< Pin for the decimal point. */
    ESD_Pin_t dot;      /*!< Pin for the 'dot' (often the colon). */
} ESD_Config_t;

/* Public Enums --------------------------------------------------------------*/

/**
 * @brief Enumeration for the displayable digits.
 */
typedef enum {
  ESD_DIGIT_0, ESD_DIGIT_1, ESD_DIGIT_2, ESD_DIGIT_3, ESD_DIGIT_4,
  ESD_DIGIT_5, ESD_DIGIT_6, ESD_DIGIT_7, ESD_DIGIT_8, ESD_DIGIT_9
} esd_digit_t;

/**
 * @brief Enumeration for the digit positions on the display.
 */
typedef enum {
  ESD_POSITION_1,   /*!< Display on the first digit (left). */
  ESD_POSITION_2,   /*!< Display on the second digit. */
  ESD_POSITION_3,   /*!< Display on the third digit. */
  ESD_POSITION_4,   /*!< Display on the fourth digit (right). */
  ESD_POSITION_ALL  /*!< Display on all digits simultaneously. */
} esd_position_t;

/* Public Function Prototypes ------------------------------------------------*/

/**
 * @brief Initializes the ESD module with a specific pin configuration.
 * @param config Pointer to the pin configuration structure.
 * @note  This function must be called before any other function in the module.
 *        It handles the initialization of the GPIO pins.
 */
void esd_init(ESD_Config_t* config);

/**
 * @brief Displays a digit at a given position on the display.
 * @param digit The digit to display (from ESD_DIGIT_0 to ESD_DIGIT_9).
 * @param pos The position where to display the digit (from ESD_POSITION_1 to ESD_POSITION_4).
 */
void esd_show_digit(esd_digit_t digit, esd_position_t pos);

#endif /* ESD_ESD_H_ */
