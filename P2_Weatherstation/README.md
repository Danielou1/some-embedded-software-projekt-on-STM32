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

While this result provides confidence that the code has been implemented correctly, it does not guarantee 100% functionality in a real-world multi-node network, as it does not test the physical layer (transceiver, bus wiring). However, the software implementation is believed to be correct and ready for integration into a physical network.
