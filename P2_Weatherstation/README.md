# P2: Weather Station with CAN Bus

## Project Overview

This project is a prototype for a weather station node designed for the STM32F429I-DISC1 board. Its primary purpose is to read environmental data from a BME280 sensor (temperature, humidity, pressure) and broadcast this data over a CAN (Controller Area Network) bus.

The system is designed to be part of a larger network of sensor nodes. This prototype initializes the necessary peripherals (I2C for the sensor, SPI for the LCD, and CAN for communication) and includes a basic framework for the final application logic.

## CAN Bus Communication

### Implementation

The CAN communication is handled by the `can_com` module. It initializes the `CAN1` peripheral with the following parameters:
- **Baud Rate**: 125 kBit/s
- **GPIO Pins**: `PB8` (RX) and `PB9` (TX)
- **Mode**: The module is currently configured in **Loopback Mode**.

### Testing and Validation

A robust test of a communication protocol like CAN requires at least two separate nodes (MCUs) to verify that messages are being sent and received correctly across the physical bus.

Due to the unavailability of a second MCU for testing, the validation of the CAN bus functionality was performed using the peripheral's built-in **Loopback Mode**. In this mode, the CAN peripheral treats its own transmitted messages as received messages, allowing for a complete self-test of the controller's logic (initialization, message transmission, filtering, and reception via interrupts).

The loopback test implemented in `main.c` was **fully functional**. A test message was successfully transmitted and immediately received, confirming that the software stack is behaving as expected.

## Real-Time Operating System (FreeRTOS)

### Integration

To handle the complexity of concurrent sensor reading, CAN communication, and UI updates, **FreeRTOS** has been integrated into the project.

- **Import Method**: Manually configured and imported as a git submodule to ensure a clean and version-controlled dependency.
- **Cleanup**: Unused port files (e.g., for non-GCC compilers or other architectures) and example folders were removed to optimize the build process and prevent linker conflicts.
- **Hardware Integration**: The FreeRTOS kernel is hooked into the STM32's interrupt system via `stm32f4xx_it.c`, managing `SVC`, `PendSV`, and `SysTick` handlers while maintaining compatibility with the STM32 HAL library.

## Upcoming Development: Multitasking Architecture

The project is now transitioning from a sequential `while(1)` loop to a priority-based multitasking architecture. The following tasks are planned:

1.  **Sensor Task (Medium Priority)**: Periodically reads data (temperature, pressure, humidity) from the BME280 sensor via I2C.
2.  **CAN Communication Task (High Priority)**: Manages the transmission of sensor data and monitors for incoming messages from other nodes.
3.  **Display/UI Task (Low Priority)**: Updates the LCD screen with local and remote sensor data, and handles joystick input for user interaction.
4.  **Watchdog Task**: Monitors system health and task execution.

This concurrent approach will ensure that time-critical operations like CAN message handling are prioritized, while providing a responsive user interface.

