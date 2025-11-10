/**
  ******************************************************************************
  * @file    	Joystick.c
  * @author		Danielou Mounsande
  * @version 	V1.0
  * @date		30.04.2021
  * @brief  	Module for using the on-board joystick of the waveshare-board.
  @verbatim
  ==============================================================================
                     ##### Usage: #####
    (#) Create a 'Joystick_Config_t' struct and fill it with the GPIO port and pins.
    (#) Initialize the corresponding GPIO pins as inputs with pull-up resistors in your main application.
    (#) Call 'joystick_init()' with your config struct to initialize the module.
    (#) Call 'joystick_read()' to read input from joystick.
    (#) Detect active bits by bitwise-compare result to the pin definitions in your config struct.
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* --- Includes */
#include "Joystick/Joystick.h"

/* --- Static module variables */
static Joystick_Config_t g_joystick_config;


/* --- Public functions */

	/**
	  * @brief  Initializes the joystick module by storing the pin configuration.
	  * @param  config: A pointer to the Joystick_Config_t struct with the pinout.
	  * @retval None
	  */
	void joystick_init(Joystick_Config_t* config) {
		g_joystick_config = *config;
	}

	/**
	  * @brief  Reads the io-pins of the joystick.
	  * @param  None
	  * @retval One or more of the bits defined in the config struct
	  *  are set when an associated button is pressed.
	  *
	  * @note	Original Joystick-Buttons are low active and therefore the result here is inverted by the '~'-operator to get a high-active result!
	  */
	uint16_t joystick_read(void) {
		uint16_t pins_all = g_joystick_config.pin_right | g_joystick_config.pin_down | g_joystick_config.pin_left | g_joystick_config.pin_up | g_joystick_config.pin_select;
		return pins_all & (~g_joystick_config.port->IDR); /* invert and return input-data-register of the port  */
	}

/* ---  End: Joystick.c */
