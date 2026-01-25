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

## Project Status & Learning Process

This project is actively being used as a learning platform for **FreeRTOS** and **Embedded Systems Architecture**. The current implementation demonstrates:
- Concurrent task management.
- Inter-task communication using **Queues**.
- Safe hardware initialization sequences.
- Transitioning from floating-point to fixed-point arithmetic for system stability.

## Roadmap & Future Improvements

To further enhance the robustness of the system and deepen the understanding of RTOS primitives, the following steps are planned:

1.  **Thread Safety with Mutexes**: Currently, shared global variables (like `g_latest_weather`) are accessed with basic `volatile` qualifiers. We plan to implement **Mutexes** to ensure atomic access and prevent data corruption during UI updates.
2.  **Resource Synchronization with Semaphores**: We intend to use **Binary or Counting Semaphores** to synchronize tasks with hardware interrupts (e.g., waking up the CAN task only when a specific hardware event occurs).
3.  **Dynamic Task Management**: Exploring the dynamic creation and deletion of tasks based on system events or user input from the joystick.
4.  **Watchdog Task**: Implementing a system health monitor to detect and recover from task stalls.

This iterative approach allows for a solid understanding of each RTOS component before moving to more complex synchronization patterns.

