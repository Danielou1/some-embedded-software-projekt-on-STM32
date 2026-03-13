# Peripheral Drivers & Utility Modules

## Architecture

This repository follows a modular, **Hardware Abstraction Layer (HAL)** approach. All hardware-specific logic is encapsulated within self-contained modules in this directory. This design choice ensures that:

1.  **Code is Reusable**: Modules can be dropped into new projects with minimal configuration.
2.  **Logic is Testable**: Core algorithms (e.g., PI control, filtering) can be unit-tested on a host machine by mocking the hardware-dependent headers.
3.  **Drivers are Clean**: Low-level register access or HAL library calls are hidden from the application logic (`main.c`).

## Module Catalog

### Control & Processing
*   **`pi_controller`**: Industrial PI controller with anti-windup.
*   **`median`**: Median filter for noise and outlier rejection.
*   **`timer_utils`**: Abstractions for non-blocking timing operations.

### Actuators
*   **`pwm_dimmer`**: Hardware PWM driver for LEDs and Fan control.
*   **`esd`**: 7-Segment Display (ESD) driver for numeric output.

### Sensors
*   **`env_sensor`**: Professional BME280 driver (I2C) with fixed-point output.
*   **`tacho_reader`**: Fan speed measurement using hardware timers.
*   **`potis`**: High-level ADC wrapper for reading potentiometers.
*   **`joystick`**: Digital input driver for user interaction.

### Communication & UI
*   **`can_com`**: Industrial CAN 2.0B driver with interrupt-driven RX.
*   **`lcd` / `my_lcd`**: Modular display drivers for the ILI9341 LCD.

---

## Connect With Me

*   **LinkedIn:** [linkedin.com/in/danielou-mounsande](https://www.linkedin.com/in/danielou-mounsande)
*   **3D Portfolio:** [danielou.netlify.app](https://danielou.netlify.app/)
*   **Email:** [mounsandedaniel@gmail.com](mailto:mounsandedaniel@gmail.com)

---
*Clean code is the foundation of high-performance embedded systems.*
