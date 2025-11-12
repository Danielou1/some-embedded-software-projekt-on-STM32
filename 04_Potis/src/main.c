/**
 ******************************************************************************
 * @file    main.c
 * @author  Danielou Mounsande
 * @version V1.1
 * @date    last update 12-November-2025
 * @brief   Main program for the 04_Potis project.
 *
 * @note    This project demonstrates reading potentiometer values using the ADC
 *          in Polling mode. In this mode, the CPU actively requests a
 *          conversion and waits for it to complete, which is simple but
 *          inefficient as it blocks the CPU.
 *
 * @section WIRING Wiring Instructions
 * This project uses the `potis` module. Connect two 10k Ohm potentiometers as follows:
 * - **Potentiometer 1 (POTI_1):**
 *   - Wiper (middle pin) -> PA6 (ADC1_IN6)
 *   - One side pin -> 3.3V
 *   - Other side pin -> GND
 *
 * - **Potentiometer 2 (POTI_2):**
 *   - Wiper (middle pin) -> PA7 (ADC1_IN7)
 *   - One side pin -> 3.3V
 *   - Other side pin -> GND
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "my_lcd/my_lcd.h" // Custom LCD module for display operations
#include "potis/potis.h"    // Potentiometer module for ADC readings
#include <stdio.h>          // Standard I/O for string formatting (sprintf)

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Converts a 12-bit ADC value (0-4095) to millivolts (0-3300mV).
 * @param adc_value The 12-bit ADC value obtained from the ADC peripheral.
 * @return The corresponding voltage in millivolts (0-3300mV).
 * @note Assumes a reference voltage (Vref) of 3.3V and a 12-bit ADC resolution.
 */
uint32_t adc_to_millivolts(uint32_t adc_value) {
    // Calculation: (ADC_Value * Vref_mV) / Max_ADC_Value
    return (adc_value * 3300U) / 4095U;
}

/* Main program --------------------------------------------------------------*/

/**
 * @brief The application entry point.
 * @retval int
 */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    HAL_Init(); // Initialize the HAL (Hardware Abstraction Layer)

    /* Initialize all configured peripherals -----------------------------------*/
    my_lcd_init(); // Initialize the custom LCD module
    // Initialize potentiometers in Polling mode.
    // In this mode, each call to potis_get_val() will trigger a new conversion
    // and block the CPU until the result is ready.
    potis_init(POTI_MODE_POLLING);

    /* Variable declarations for display ---------------------------------------*/
    char buf[32]; // Buffer to hold formatted text for LCD display
    uint32_t poti1_val, poti2_val; // Raw 12-bit ADC values
    uint32_t poti1_mv, poti2_mv;   // Potentiometer values in millivolts

    /* Initial LCD display setup -----------------------------------------------*/
    my_lcd_draw_text_at_line("Poti 1:", 1, BLACK, 2, WHITE); // Label for Potentiometer 1
    my_lcd_draw_text_at_line("Poti 2:", 6, BLACK, 2, WHITE); // Label for Potentiometer 2

    /* Infinite loop -----------------------------------------------------------*/
    while(1) {
        /* Potentiometer 1 processing */
        // Read potentiometer 1 value. In Polling mode, this function blocks until the conversion is done.
        poti1_val = potis_get_val(POTI_1);
        poti1_mv = adc_to_millivolts(poti1_val); // Convert raw ADC value to millivolts

        // Display Potentiometer 1 bargraph. The value is scaled to 0-1000 for the bargraph function.
        my_lcd_draw_bargraph(0, 30, 240, 20, (poti1_val * 1000U) / 4095U, BLUE, LIGHTGREY);
        // Display Potentiometer 1 textual value in millivolts
        sprintf(buf, "%4lu mV", poti1_mv);
        my_lcd_draw_text_at_line(buf, 3, BLACK, 2, WHITE);

        /* Potentiometer 2 processing */
        // Read potentiometer 2 value.
        poti2_val = potis_get_val(POTI_2);
        poti2_mv = adc_to_millivolts(poti2_val); // Convert raw ADC value to millivolts

        // Display Potentiometer 2 bargraph.
        my_lcd_draw_bargraph(0, 150, 240, 20, (poti2_val * 1000U) / 4095U, RED, LIGHTGREY);
        // Display Potentiometer 2 textual value in millivolts
        sprintf(buf, "%4lu mV", poti2_mv);
        my_lcd_draw_text_at_line(buf, 8, BLACK, 2, WHITE);

        // Small delay to slow down the loop.
        HAL_Delay(100);
    }
}
