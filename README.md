# Advanced Embedded C Projects on STM32

[![Status](https://img.shields.io/badge/Status-In_Progress-blue)](https://github.com/Danielou1/some-embedded-software-projekt-on-STM32)
[![Platform](https://img.shields.io/badge/Platform-STM32F429-orange)](https://www.st.com/en/microcontrollers-microprocessors/stm32f429-439.html)
[![Language](https://img.shields.io/badge/Language-C_(HAL)-green)](https://www.st.com/en/embedded-software/stm32cube-mcu-mpu-packages.html)
[![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-red)](https://www.freertos.org/)
[![Tests](https://img.shields.io/badge/Tests-Unity-brightgreen)](https://github.com/ThrowTheSwitch/Unity)

## Introduction

This repository is a curated collection of embedded systems projects developed on the **STM32F429** platform. It documents the journey from foundational GPIO control to advanced Real-Time Operating System (RTOS) architectures. Each project has been refactored to meet industrial software engineering standards, focusing on modularity, robustness, and clean code.

The repository serves as a showcase of technical progression, demonstrating mastery over low-level hardware (I2C, CAN, SPI, DMA) and high-level concurrency management (FreeRTOS).

## Core Philosophy & Best Practices

*   **Modular, Reusable Drivers:** Hardware-specific logic is encapsulated into self-contained modules located in the `/modules` directory.
*   **Event-Driven Architecture:** Moving away from polling to interrupt-driven and event-based systems for maximum CPU efficiency.
*   **RTOS Primitives:** Using Mutexes for resource protection and Semaphores for task synchronization to ensure thread safety.
*   **Professional Documentation:** Full English documentation using Doxygen style, including detailed architectural explanations.
*   **Deterministic Execution:** Use of fixed-point arithmetic and careful stack management for high-reliability applications.
*   **Automated Unit Testing:** Utilizing the Unity framework to verify module logic independently of hardware using mocks.

---

## Projects Overview

| Project | Title | Key Concepts & Technologies |
| :--- | :--- | :--- |
| **`00_Introduction`** | GPIO Control | Basic HAL library, GPIO output configuration. |
| **`01_ESD`** | 7-Segment Display | Complex GPIO management, modular driver design. |
| **`02_Joystick`** | Digital Input | GPIO input, debouncing, state machine logic. |
| **`03_LCD`** | Display Driver | SPI/Parallel communication, hardware abstraction. |
| **`04_Potis`** | ADC Polling | Analog-to-Digital Conversion (ADC) basic reading. |
| **`05_Potis_DMA`** | ADC with DMA | High-efficiency ADC data transfer using DMA. |
| **`06_Blinky_Dot`** | Hardware PWM | PWM generation using hardware timers (1Hz). |
| **`07_Dimming_Dot`** | PWM Control | ADC-to-PWM mapping, real-time user interaction. |
| **`08_Stopwatch`** | EXTI Timer | External Interrupts (EXTI), precise timekeeping. |
| **`P1_Fan_Control`** | PI Controller | Closed-loop control, Tachometer reading, PWM actuators. |
| **`P2_Weatherstation`**| **RTOS Weather Node** | **FreeRTOS (V10.x), Mutex, Semaphores, CAN Bus (Interrupt-driven), I2C, Fixed-Point.** |

### Key Modules Developed

*   **`can_com`**: Industrial CAN 2.0B driver with interrupt-driven RX and error handling.
*   **`env_sensor`**: Professional BME280 driver using I2C with Doxygen-level documentation.
*   **`pi_controller`**: Generic PI controller with Anti-Windup for industrial control loops.
*   **`timer_utils`**: Non-blocking millisecond timer abstraction.
*   **`lcd`**: Modular display driver protected by RTOS Mutexes.

---

## Testing Infrastructure

To ensure the reliability of the core logic, a unit testing suite has been implemented using the **Unity** framework. Tests are executed on the host machine using mocks for hardware-dependent headers (e.g., `stm32f4xx.h`).

*   **Framework:** Unity Test
*   **Location:** `/tests`
*   **Mocks:** Hardware abstractions located in `/tests/mocks`
*   **Execution:** 
    *   **Windows:** Run `make` (ensure MinGW/GCC is in your PATH).
    *   **Linux:** Run `make`. The Makefile automatically detects the OS and adjusts commands accordingly.

---

## Roadmap & Future Work

*   **Expand Test Coverage:** Implement unit tests for all core modules in the `/modules` directory.
*   **LVGL Integration:** Implement the Light and Versatile Graphics Library for professional UI on project P2.
*   **Low-Power Optimization:** Implement Tickless Idle mode and hardware sleep states.
*   **Physical CAN Networking:** Transition from Loopback to Normal mode with multiple physical nodes.
*   **Data Logging:** Add SD Card / FATFS support for long-term environmental data storage.

### Recent Milestones
- [x] **Unit Testing Infrastructure:** Introduced Unity framework with hardware mocking for off-target verification.
- [x] **RTOS Integration:** Successfully migrated P2 to a fully event-driven FreeRTOS architecture.
- [x] **Concurrency Safety:** Implemented Mutexes and Semaphores for thread-safe resource sharing.
- [x] **Industrial Docs:** Completed comprehensive Doxygen documentation for the entire project stack.

## Connect With Me

I am an aspiring Embedded Systems Engineer focused on building robust, real-time software solutions.

*   **LinkedIn:** [linkedin.com/in/danielou-mounsande](https://www.linkedin.com/in/danielou-mounsande)
*   **3D Portfolio:** [danielou.netlify.app](https://danielou.netlify.app/)
*   **Email:** [danieloumounsande@hotmail.com](mailto:danieloumounsande@hotmail.com)

---
*License: MIT*