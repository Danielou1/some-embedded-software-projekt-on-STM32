#include "lcd/lcd.h"
#include "stm32f4xx.h"
#include "env_sensor/env_sensor.h"
#include <stdio.h>

int main(void)
{
	HAL_Init();

	/* Initialization of the LCD */
	lcd_init();
	lcd_fill_screen(WHITE);

	/* Draw text to LCD */
	lcd_draw_text_at_line("Wetterstation", 2, BLACK, 2, WHITE);

	/* Initialization of the BME280 Sensor */
	if (env_sensor_init() != BME280_OK)
	{
		lcd_draw_text_at_line("BME280 Init Failed!", 4, RED, 2, WHITE);
		while(1); // Stop here
	}
	else
	{
		lcd_draw_text_at_line("BME280 Initialized", 4, GREEN, 2, WHITE);
	}

	HAL_Delay(1000); // Wait a moment before starting measurements
	lcd_draw_rect(0, 4 * 16, 240, (4 * 16) + 16, WHITE, 1); // Clear the init message line


	struct bme280_data comp_data;
	char temp_str[32];
	char press_str[32];
	char hum_str[32];

	while(1)
	{
		if (env_sensor_read_data(&comp_data) == BME280_OK)
		{
			// Format the data into strings
			// Note: The BME280 library provides pressure in Pa. We convert to hPa.
			// Temperature is in DegC. Humidity is in %RH.
			sprintf(temp_str, "Temp:  %.2f C", comp_data.temperature);
			sprintf(press_str, "Press: %.2f hPa", comp_data.pressure / 100.0);
			sprintf(hum_str, "Hum:   %.2f %%", comp_data.humidity);

			// Display the data on the LCD
			lcd_draw_text_at_line(temp_str, 6, BLACK, 2, WHITE);
			lcd_draw_text_at_line(press_str, 8, BLACK, 2, WHITE);
			lcd_draw_text_at_line(hum_str, 10, BLACK, 2, WHITE);
		}
		else
		{
			lcd_draw_text_at_line("BME280 Read Failed!", 12, RED, 2, WHITE);
		}

		HAL_Delay(1000); // Wait for 1 second
	}
}
