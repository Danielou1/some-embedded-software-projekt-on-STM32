# P1: Closed-Loop Fan Speed Control with PI Controller

## Project Overview

This project implements a professional **Closed-Loop Control System** for a DC fan on the STM32F429 platform. It combines multiple modular drivers to maintain a stable fan speed (RPM) regardless of external disturbances.

The system uses a **Proportional-Integral (PI) Controller** to adjust the PWM duty cycle based on the difference between the user-defined target speed and the real-time measured speed.

## Core Components & Architecture

*   **PI Controller (`pi_controller`)**: A robust control algorithm with built-in **Anti-Windup** logic to prevent integral saturation and ensure rapid recovery from disturbances.
*   **Tachometer Reader (`tacho_reader`)**: Uses input capture or frequency measurement to calculate the real-time RPM of the fan.
*   **PWM Dimmer (`pwm_dimmer`)**: Controls the power delivered to the fan motor using hardware timers.
*   **ADC Interface (`potis`)**: Reads an external potentiometer to set the target RPM (0 - 5000 RPM).
*   **LCD Interface (`my_lcd`)**: Provides a real-time visualization of target RPM, actual RPM, and controller output (Duty Cycle).

## Control Loop

The control loop is executed periodically every **100ms** using a hardware timer interrupt (**TIM3**). This ensures a deterministic and stable control frequency, which is critical for industrial applications.

1.  **Input Acquisition**: Read target RPM from the potentiometer.
2.  **Measurement**: Get current RPM from the tachometer module.
3.  **Calculation**: Compute the new duty cycle using the PI algorithm.
4.  **Actuation**: Apply the new PWM duty cycle to the fan.
5.  **Feedback**: Update the LCD with current system status.

## Technical Specifications

*   **Platform**: STM32F429I-DISC1
*   **Control Frequency**: 10 Hz (100ms interval)
*   **PWM Frequency**: 25 kHz (standard for fan control)
*   **Control Range**: 20% to 100% duty cycle (to prevent motor stalling)
*   **Target RPM Range**: 0 to 5000 RPM

---

## Connect With Me

*   **LinkedIn:** [linkedin.com/in/danielou-mounsande](https://www.linkedin.com/in/danielou-mounsande)
*   **3D Portfolio:** [danielou.netlify.app](https://danielou.netlify.app/)
*   **Email:** [mounsandedaniel@gmail.com](mailto:mounsandedaniel@gmail.com)

---
*This project serves as a demonstration of closed-loop control principles and modular software architecture.*
