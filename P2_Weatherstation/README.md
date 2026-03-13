# P2: Weather Station with CAN Bus & FreeRTOS

## Project Overview

This project is a high-reliability weather monitoring system for the STM32F429I-DISC1 board. It integrates environmental data acquisition (BME280), industrial communication (CAN Bus), and a real-time reactive UI.

The system is built on **FreeRTOS** and follows an **Event-Driven Architecture** to ensure maximum CPU efficiency and system robustness.

## Version 3.6 - Industrial Implementation (Current)

The current version (V3.6) represents a significant architectural shift from polling to a fully reactive, interrupt-driven system.

### Core Features
*   **Interrupt-Driven UI**: The UI task is event-based, staying in a `Blocked` state until triggered by CAN reception or hardware errors.
*   **Resource Protection**: Implementation of **Mutexes** for the LCD and **Binary Semaphores** for task synchronization.
*   **Fixed-Point Arithmetic**: Optimized data processing (int32_t x100) to ensure system stability and FPU efficiency.
*   **Modular Design**: Dedicated `can_com` driver module handling low-level hardware configuration (Filters, Interrupts, MSP).

## Roadmap & Future Improvements

To further evolve this prototype into a production-ready industrial sensor node, the following features are planned:

### 1. Physical Multi-Node Networking
*   Transition from **Loopback Mode** to **Normal Mode**.
*   Connect multiple STM32 nodes using physical CAN transceivers (e.g., TJA1050) to test bus arbitration and collision handling.

### 2. Advanced Graphics with LVGL
*   Integrate the **LVGL (Light and Versatile Graphics Library)** to replace basic text output with professional UI elements.
*   Implement real-time line charts to visualize temperature and pressure trends over time.

### 3. Power Optimization (Tickless Idle)
*   Implement **FreeRTOS Tickless Idle mode** combined with STM32 low-power modes (Sleep/Stop).
*   Reduce power consumption during the 1-second interval between sensor samples.

### 4. Data Logging & Storage
*   Add an SD card module (via SPI/SDIO) to log environmental data in CSV format.
*   Implement a FATFS file system task for long-term data storage and retrieval.

### 5. Hardware Watchdog (IWDG)
*   Add a dedicated Hardware Watchdog task to ensure the system reboots automatically in case of a catastrophic software hang, complementing the existing software CAN watchdog.

---

## Connect With Me

*   **LinkedIn:** [linkedin.com/in/danielou-mounsande](https://www.linkedin.com/in/danielou-mounsande)
*   **3D Portfolio:** [danielou.netlify.app](https://danielou.netlify.app/)
*   **Email:** [mounsandedaniel@gmail.com](mailto:mounsandedaniel@gmail.com)

---
*This project serves as a demonstration of advanced Embedded RTOS concepts, focusing on resource protection, interrupt synchronization, and modular driver design.*
