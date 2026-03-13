# Unit Testing Suite (Unity & Hardware Mocks)

## Overview

This directory contains the automated unit testing suite for the repository's core modules. To ensure high reliability and modularity, logic-heavy components (like filters and controllers) are tested independently of the target hardware.

The suite uses the **Unity Test Framework** and a **Mocking** approach to simulate hardware-dependent headers.

## Testing Strategy

*   **Off-Target Execution**: Tests are compiled and executed on the host development machine (Windows/Linux) rather than the STM32 microcontroller. This allows for extremely fast feedback loops.
*   **Hardware Mocking**: Files in `/tests/mocks` (e.g., `stm32f4xx.h`) provide the necessary definitions for the code to compile on the host system without including the massive STM32 HAL libraries.
*   **Memory Safety**: Tests verify that modules correctly handle edge cases, buffer overflows, and numerical stability (e.g., PI controller anti-windup).

## Current Test Modules

1.  **`test_median`**: Verifies the median filter's ability to reject outliers and smooth signals.
2.  **`test_pi_controller`**: Validates the Proportional-Integral control logic, output clamping, and anti-windup functionality.

## How to Run Tests

### Prerequisites
*   **GCC**: A C compiler must be installed on your host machine (e.g., MinGW for Windows, `build-essential` for Linux).
*   **Make**: Used to automate the build and execution process.

### Execution
Navigate to this directory and run:

```bash
make
```

To clean build artifacts:

```bash
make clean
```

---

## Connect With Me

*   **LinkedIn:** [linkedin.com/in/danielou-mounsande](https://www.linkedin.com/in/danielou-mounsande)
*   **3D Portfolio:** [danielou.netlify.app](https://danielou.netlify.app/)
*   **Email:** [mounsandedaniel@gmail.com](mailto:mounsandedaniel@gmail.com)

---
*Testing is the backbone of reliable embedded software engineering.*
