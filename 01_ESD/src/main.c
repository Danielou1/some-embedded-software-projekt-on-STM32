/**
  ******************************************************************************
  * @file    	main.c
  * @author		Danielou Mounsande
  * @version 	V1.0
  * @date       25.04.2024
  ******************************************************************************
*/

#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include "esd/esd.h"
#include "delay_utils/delay_utils.h"
int main(void) {
	HAL_Init();

	esd_init();

	esd_digit_t digitArray[10] = { ESD_DIGIT_0, ESD_DIGIT_1, ESD_DIGIT_2,
			ESD_DIGIT_3, ESD_DIGIT_4, ESD_DIGIT_5, ESD_DIGIT_6, ESD_DIGIT_7,
			ESD_DIGIT_8, ESD_DIGIT_9 };
	esd_position_t positionArray[5] = { ESD_POSITION_1, ESD_POSITION_2,
			ESD_POSITION_3, ESD_POSITION_4, ESD_POSITION_ALL };
	int digitCounter = 9;
	int posCounter = 0;

	while (1) {

		esd_show_digit(digitArray[digitCounter], positionArray[posCounter]);
		utils_delay_ms(1000);
		digitCounter--;
		if(digitCounter < 0){
			posCounter++;
			digitCounter = 9;
		}
		if(posCounter == 5) posCounter = 0;

	}
}




