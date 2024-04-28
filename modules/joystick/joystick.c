/**
  ******************************************************************************
  * @file    	Joystick.c
  * @author		Danielou Mounsande
  * @version 	V1.0
  * @date		30.04.2021
  * @brief  	Module for using the on-board joystick of the waveshare-board.
  @verbatim
  ==============================================================================
                    ##### resources used #####
	GPIO: Like described in the port- and pin-map in joystik.h (#define JOYSTICK_PORT_*, JOYSTICK_PIN_*)
  ==============================================================================
                     ##### Usage: #####
    (#) Call 'joystick_init()' function to initialize the module.
    (#) Call 'joystick_read()' to read input from joystick.
    (#) Detect active bits by bitwise-compare result to JOYSTICK_PIN_UP, _LEFT, ...-Defines.
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* --- Includes */
#include "Joystick/Joystick.h"

/* --- Preprocessor defines */
	/* - none - */
/* --- Preprocessor macros */
	/* - none - */

/* --- Module intern type definitions */
	/* - none - */

/* Static module variables */
	/* - none - */

/* private (Static) module functions (prototypes) */
	static void joystick_RCC_Config( void );
	static void joystick_GPIO_Config( void );


/* --- Public functions */

	/**
	  * @brief  Initializes the joystick-port / -pins.
	  * @param  None
	  * @retval None
	  */
	void joystick_init(void) {
		joystick_RCC_Config();
		joystick_GPIO_Config();
	}

	/**
	  * @brief  Reads the io-pins of the joystick.
	  * @param  None
	  * @retval One or more of the bits defined in
	  *  'JOYSTICK_PIN_UP, _DOWN, _LEFT, _RIGHT or _SELECT'
	  *  are set when an associated button is pressed.
	  *
	  * @note	Original Joystick-Buttons are low active and therefore the result here is inverted by the '~'-operator to get a high-active result!
	  */
	uint16_t joystick_read(void) {
		return JOYSTICK_PIN_ALL & (~JOYSTICK_PORT->IDR); /* invert and return input-data-register of the port  */
	}


/* --- private (Static) module functions (implementation) */

	/**
	  * @brief  Configures clock for joystick-gpio-port.
	  * @param  None
	  * @retval None
	  */
	static void joystick_RCC_Config( void ) {
		__HAL_RCC_GPIOG_CLK_ENABLE();
	}

	/**
	  * @brief  Configures gpio-pins for joystick-gpio-port.
	  * @param  None
	  * @retval None
	  */
	static void joystick_GPIO_Config( void ) {
		GPIO_InitTypeDef gpio_init_joy = { 0 }; /* set all fields to 0 */

		/* !! Note: The joystick-buttons are (low-active) switches that connect a gpio-pin to ground if pressed.
		 * If no button is pressed, the pin has to be pulled to a defined logic-level by activating a pull-up-resistor on the input-pin!! */

		/* Pin-configuration as inputs with pull-up-resistors */
		gpio_init_joy.Pin = JOYSTICK_PIN_ALL;
		gpio_init_joy.Mode = GPIO_MODE_INPUT;
		gpio_init_joy.Pull = GPIO_PULLUP;

		HAL_GPIO_Init(JOYSTICK_PORT, &gpio_init_joy);
	}

/* ---  End: Joystick.c */
