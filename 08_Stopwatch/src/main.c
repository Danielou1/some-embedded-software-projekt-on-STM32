/**
 ******************************************************************************
 * @file    	main.c
 * @author		Danielou Mounsande
 * @version 	V1.1
 * @date		update on 14-November-2025
 * @brief		Main program for the 08_Stopwatch project.
 *
 * @note    This project implements a fully functional lap timer as per the
 *          requirements of "Abgabe 3, Task 7".
 *          - User Button (Blue): Starts the timer and records laps.
 *          - Joystick Down: Pauses the timer.
 *          - Joystick Center: Resets the timer.
 *          It uses the `stopwatch`, `timer_utils`, `my_lcd`, and `joystick` modules.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>
#include "my_lcd/my_lcd.h"
#include "timer_utils/timer_utils.h"
#include "stopwatch/stopwatch.h"
#include "joystick/joystick.h"

/* Private enums -------------------------------------------------------------*/
// Note: This logic is application-specific and not part of the generic joystick module.
typedef enum {
    JOYSTICK_NONE,
    JOYSTICK_UP,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_CENTER
} Joystick_Direction_t;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void format_time(char* buf, uint32_t time_ms);
static Joystick_Direction_t joystick_get_direction_debounced(void);

// Joystick configuration defined globally to be accessible by the debouncer function
static Joystick_Config_t joy_config;

/* Main Program --------------------------------------------------------------*/

/**
 * @brief  Application entry point.
 * @retval int
 */
int main(void)
{
	// Hardware and module initialization
	HAL_Init();
	SystemClock_Config();
	SystemCoreClockUpdate();
	my_lcd_init();
	timer_utils_init();

	// Configure and initialize the joystick
	joy_config.port = GPIOG;
	joy_config.pin_select = GPIO_PIN_6;
	joy_config.pin_left = GPIO_PIN_9;
	joy_config.pin_down = GPIO_PIN_10;
	joy_config.pin_right = GPIO_PIN_11;
	joy_config.pin_up = GPIO_PIN_12;
	joystick_init(&joy_config);

	stopwatch_init(); // This configures the user button interrupt

	// Application variables
	char main_time_buf[32];
	char lap_time_buf[32];

	// Initial display
	my_lcd_draw_text_at_line("Stopwatch Ready", 2, BLACK, 2, WHITE);
	my_lcd_draw_text_at_line("BTN:Start/Lap", 10, DARKGREY, 2, WHITE);
	my_lcd_draw_text_at_line("JOY:Pause/Reset", 11, DARKGREY, 2, WHITE);


	while(1) {
		// 1. Handle Joystick input for Pause and Reset
		Joystick_Direction_t dir = joystick_get_direction_debounced();
		if (dir == JOYSTICK_DOWN) {
			stopwatch_pause();
		} else if (dir == JOYSTICK_CENTER) {
			stopwatch_reset();
		}

		// 2. Get current times from the stopwatch module
		uint32_t elapsed_ms = stopwatch_get_elapsed_ms();
		uint8_t lap_count = stopwatch_get_lap_count();

		// 3. Format and display the main time
		format_time(main_time_buf, elapsed_ms);
		my_lcd_draw_text_at_line(main_time_buf, 4, BLACK, 4, WHITE);

		// 4. Format and display the last lap time
		if (lap_count > 0) {
			uint32_t last_lap_ms = stopwatch_get_lap_ms(lap_count - 1);
			sprintf(lap_time_buf, "Lap %d:", lap_count);
			my_lcd_draw_text_at_line(lap_time_buf, 5, DARKGREY, 2, WHITE);

			format_time(lap_time_buf, last_lap_ms);
			my_lcd_draw_text_at_line(lap_time_buf, 6, DARKGREY, 2, WHITE);
		} else {
			// Clear lap time display when reset
			my_lcd_draw_text_at_line("                ", 7, BLACK, 2, WHITE);
			my_lcd_draw_text_at_line("                ", 8, BLACK, 2, WHITE);
		}

		// Small delay to prevent screen tearing/flickering
		HAL_Delay(50);
	}
}

/**
 * @brief  Reads the joystick direction with simple debouncing.
 * @note   This is a blocking function that waits for the button to be released.
 * @retval The direction of the joystick.
 */
static Joystick_Direction_t joystick_get_direction_debounced(void)
{
    uint16_t current_state = joystick_read();
    Joystick_Direction_t direction = JOYSTICK_NONE;

    if (current_state != 0) // A button is pressed
    {
        // Wait for the button to be released (simple debounce)
        while (joystick_read() != 0);

        // Determine which button was pressed
        if (current_state & joy_config.pin_up) {
            direction = JOYSTICK_UP;
        } else if (current_state & joy_config.pin_down) {
            direction = JOYSTICK_DOWN;
        } else if (current_state & joy_config.pin_left) {
            direction = JOYSTICK_LEFT;
        } else if (current_state & joy_config.pin_right) {
            direction = JOYSTICK_RIGHT;
        } else if (current_state & joy_config.pin_select) {
            direction = JOYSTICK_CENTER;
        }
    }
    return direction;
}

/**
 * @brief  Formats a time in milliseconds to a MM:SS.ms string.
 * @param  buf: The output character buffer.
 * @param  time_ms: The time in milliseconds to format.
 * @retval None
 */
static void format_time(char* buf, uint32_t time_ms)
{
	uint16_t minutes = time_ms / 60000;
	uint16_t seconds = (time_ms % 60000) / 1000;
	uint16_t milliseconds = time_ms % 1000;
	sprintf(buf, "%02d:%02d.%03d", minutes, seconds, milliseconds);
}

/**
 * @brief  System Clock Configuration.
 * @retval None
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}
