/**
  ******************************************************************************
  * @file    	esd.c
  * @author		Danielou Mounsande
  * @version 	V1.0
  * @date       18.04.2024
  ******************************************************************************
*/
#include "stm32f4xx.h"
/* Includes */
#include "esd/esd.h"

// Static variable to hold the configuration
static ESD_Config_t g_esd_config;

// Helper array for segment mapping (0-9) for a common anode display
// The logic is inverted: 0 means the segment is ON, 1 means OFF.
// Segments: A, B, C, D, E, F, G
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


void esd_init(ESD_Config_t* config){
	// Store the configuration
	g_esd_config = *config;
	// Note: GPIOs must be initialized in the main application
}
void esd_show_digit(esd_digit_t digit, esd_position_t pos) {

	// Turn off all segments first (set to 1 for common anode)
	HAL_GPIO_WritePin(g_esd_config.led_a.port, g_esd_config.led_a.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(g_esd_config.led_b.port, g_esd_config.led_b.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(g_esd_config.led_c.port, g_esd_config.led_c.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(g_esd_config.led_d.port, g_esd_config.led_d.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(g_esd_config.led_e.port, g_esd_config.led_e.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(g_esd_config.led_f.port, g_esd_config.led_f.pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(g_esd_config.led_g.port, g_esd_config.led_g.pin, GPIO_PIN_SET);

	// Turn off all position controllers
	HAL_GPIO_WritePin(g_esd_config.cntl1.port, g_esd_config.cntl1.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(g_esd_config.cntl2.port, g_esd_config.cntl2.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(g_esd_config.cntl3.port, g_esd_config.cntl3.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(g_esd_config.cntl4.port, g_esd_config.cntl4.pin, GPIO_PIN_RESET);


	// Light up the segments for the digit
	if (digit >= ESD_DIGIT_0 && digit <= ESD_DIGIT_9) {
		if (!digit_map[digit][0]) HAL_GPIO_WritePin(g_esd_config.led_a.port, g_esd_config.led_a.pin, GPIO_PIN_RESET);
		if (!digit_map[digit][1]) HAL_GPIO_WritePin(g_esd_config.led_b.port, g_esd_config.led_b.pin, GPIO_PIN_RESET);
		if (!digit_map[digit][2]) HAL_GPIO_WritePin(g_esd_config.led_c.port, g_esd_config.led_c.pin, GPIO_PIN_RESET);
		if (!digit_map[digit][3]) HAL_GPIO_WritePin(g_esd_config.led_d.port, g_esd_config.led_d.pin, GPIO_PIN_RESET);
		if (!digit_map[digit][4]) HAL_GPIO_WritePin(g_esd_config.led_e.port, g_esd_config.led_e.pin, GPIO_PIN_RESET);
		if (!digit_map[digit][5]) HAL_GPIO_WritePin(g_esd_config.led_f.port, g_esd_config.led_f.pin, GPIO_PIN_RESET);
		if (!digit_map[digit][6]) HAL_GPIO_WritePin(g_esd_config.led_g.port, g_esd_config.led_g.pin, GPIO_PIN_RESET);
	}

	// Select the position
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
