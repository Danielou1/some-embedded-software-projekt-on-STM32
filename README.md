# Advanced Embedded C Projects on STM32

[![Status](https://img.shields.io/badge/Status-In_Progress-blue)](https://github.com/Danielou1/some-embedded-software-projekt-on-STM32)
[![Platform](https://img.shields.io/badge/Platform-STM32F429-orange)](https://www.st.com/en/microcontrollers-microprocessors/stm32f429-439.html)
[![Language](https://img.shields.io/badge/Language-C_(HAL)-green)](https://www.st.com/en/embedded-software/stm32cube-mcu-mpu-packages.html)

## Introduction

This repository is a curated collection of embedded systems projects developed on the **STM32F429** platform. It documents the process of refactoring and enhancing foundational university and personal projects—some of which are more than two years old—with advanced, industry-standard software engineering practices. This repository serves as a living document of skill progression and the application of modern embedded principles to older codebases.

The primary objective is to demonstrate a deep understanding of both low-level hardware interaction and high-level software architecture, showcasing a commitment to writing clean, robust, and maintainable embedded code.

## Core Philosophy & Best Practices

Each project in this repository is being systematically improved to adhere to the following principles:

*   **Modular, Reusable Drivers:** Hardware-specific logic is encapsulated into self-contained, reusable modules (e.g., for displays, sensors, and peripherals). This promotes code reuse and separation of concerns.
*   **Hardware Abstraction:** Application logic in `main.c` is kept clean and focused on the "what," while the underlying modules handle the "how."
*   **Professional Documentation:** All code is documented in English using Doxygen-style comments. File headers include detailed wiring instructions and explanations of the project's purpose.
*   **Non-Blocking Code:** Where applicable, projects are designed to be non-blocking, using hardware timers and interrupts instead of blocking delays (`HAL_Delay`) to ensure an efficient and responsive system.
*   **Configuration-Driven:** Modules are designed to be configured via structs, making them adaptable to different hardware configurations without changing the module's source code.

---

## Completed Projects Showcase

The following projects have been refactored to meet the standards outlined above.

| Project | Title | Key Concepts Demonstrated |
| :--- | :--- | :--- |
| **`00_Introduction`** | GPIO Control | Basic HAL library usage, GPIO output configuration. |
| **`01_ESD`** | 7-Segment Display | Complex GPIO management, modular driver design. |
| **`02_Joystick`** | Digital Input | GPIO input with pull-up/pull-down, state machine logic. |
| **`03_LCD`** | SPI & Display Driver | SPI communication, driver wrapping for hardware abstraction. |
| **`04_Potis`** | ADC Polling | Analog-to-Digital Conversion (ADC) in polling mode. |
| **`05_Potis_DMA`** | ADC with DMA | Advanced ADC reading using Direct Memory Access (DMA) for efficiency. |
| **`06_Blinky_Dot`** | Hardware PWM Blink | Low-frequency signal generation (1Hz) using a hardware timer in PWM mode. |
| **`07_Dimming_Dot`** | Real-Time PWM Control | ADC to PWM mapping, real-time user interaction, handling active-low hardware. |
| **`08_Stopwatch`** | Lap Timer | External Interrupts (EXTI), timekeeping, state machines. |
| **`P1_Fan_Control`** | PI Speed Controller | Closed-loop control (PI), PWM, tachometer reading, modular system design. |
| **`P2_Weatherstation`** | CAN Bus Weather Node | I²C sensor integration (BME280), CAN bus setup and loopback testing, advanced debugging. |

### Key Modules Developed

*   **`esd`:** A fully configurable driver for the 8-segment LED display board.
*   **`joystick`:** A simple driver for reading the 5-way joystick.
*   **`potis`:** A driver for reading potentiometers, supporting both polling and DMA modes.
*   **`my_lcd`:** A wrapper for the base LCD driver to provide a consistent interface.
*   **`timer_utils`:** A non-blocking millisecond timer using a hardware timer and interrupts.
*   **`pwm_dimmer`:** A flexible, frequency-based PWM driver for controlling LEDs or other actuators.
*   **`stopwatch`:** A high-precision lap timer using EXTI and a hardware timer.
*   **`tacho_reader`:** A module to measure fan RPM using EXTI and a timer.
*   **`pi_controller`:** A generic Proportional-Integral (PI) controller module with anti-windup.
*   **`env_sensor`:** A driver for the BME280 environmental sensor using I²C, with Doxygen documentation.
*   **`can_com`:** A module to handle initialization and communication over the CAN bus.

---

## Roadmap & Future Work

This repository is actively being developed. The next steps include:

*   **CAN Bus Network:** Finalize the `P2_Weatherstation` project by implementing the full multi-node communication logic, including data serialization and reception from other nodes.
*   **RTOS Concepts:** Implement a simple real-time operating system (RTOS) to manage multiple tasks concurrently in a more advanced project.
*   **Advanced ARM Cortex-M4 Features:** Explore and implement projects using the Memory Protection Unit (MPU) or the Floating Point Unit (FPU).

### Completed Milestones
*   **I²C Communication:** Successfully integrated an external I²C sensor (BME280) and debugged hardware/software issues.

## Connect With Me

I am passionate about embedded systems and building robust, efficient software. Let's connect!

*   **LinkedIn:** [linkedin.com/in/danielou-mounsande](https://www.linkedin.com/in/danielou-mounsande)
*   **3D Portfolio:** [danielou.netlify.app](https://danielou.netlify.app/)
*   **Email:** [danieloumounsande@hotmail.com](mailto:danieloumounsande@hotmail.com)

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
