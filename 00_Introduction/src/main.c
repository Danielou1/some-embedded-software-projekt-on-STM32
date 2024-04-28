/**
  ******************************************************************************
  * @file    	main.c
  * @author		Danielou Mounsande...
  * @version 	V1.0
  * @date		18.04.2024
  * @brief  	Template using the LCD
  ******************************************************************************
*/


#include <lcd/lcd.h>
#include "stm32f4xx.h"



int main(void)
{
	HAL_Init();

	/* Initialization of the LCD */
	lcd_init();

    /* 1- Wie oben erläutert, ist die grüne LED an PG13 angeschlossen. An welchen Pin ist die rote LED des
Discovery Boards angeschlossen? : PG14 */

    /* 2-  Konfigurieren Sie die GPIO-Ports für den Betrieb beider LEDs.*/

	__HAL_RCC_GPIOG_CLK_ENABLE();
    GPIO_InitTypeDef gpio_init;
    		gpio_init.Pin = GPIO_PIN_14 | GPIO_PIN_13;
    		gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    		gpio_init.Pull = GPIO_NOPULL;
    		gpio_init.Speed = GPIO_SPEED_MEDIUM;
    		HAL_GPIO_Init(GPIOG, &gpio_init);

    /* 3- Aktivieren Sie nun die grüne LED. */

    	//HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);

    /* 4- Aktivieren Sie mithilfe eines einzigen Funktionsaufrufs beide LEDs. */

   		//HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);

   	/* 5- Aktivieren Sie beide LEDs und deaktivieren Sie anschließend die rote LED wieder. */

          //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);
          //HAL_Delay(1500);
   		  //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);

  	/* 6- Deaktivieren Sie mit nur einem Funktionsaufruf beide LEDs. */

    	  //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);

    /* 7- Aktivieren Sie die grüne LED und togglen Sie daraufhin beide LEDs mit der Toggle
    			 Funktion. Rufen Sie diesen Befehl mehrmals auf, um den Effekt zu sehen. */

	while(1) {
	    		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14);
	    	    HAL_Delay(1500);


	}
}
