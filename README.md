# stm32-uart-adc-control

STM32 embedded C project — control an LED via UART commands, read ADC values, and handle GPIO button input. Built with STM32 HAL on a Nucleo-F401RE board.

## Features

- **UART control** — send commands from PC to control LED behavior
- **ADC reading** — read analog input (potentiometer), output percentage via UART
- **GPIO button** — physical button press detection with debounce flag
- **Interrupt-driven** — UART TX/RX and ADC run via HAL IT callbacks (non-blocking)

## UART Command Reference

| Command | Key | Action |
|---------|-----|--------|
| `0` | 48 | LED off |
| `1` | 49 | LED on |
| `2` | 50 | LED blink (fast) |
| `3` | 51 | LED blink (slow, period=10) |
| `4` | 52 | LED blink (medium, period=5) |
| `5` | 53 | Start ADC read — prints `adc_data = X%` |

## Hardware

- **Board:** STM32 Nucleo-F401RE (or compatible STM32F4)
- **LED:** PA5 (LD2)
- **Button:** PC13 (B1)
- **ADC:** Channel 0 (PA0) — 12-bit resolution
- **UART:** USART2 @ 115200 baud (connected via ST-Link USB)

## Peripherals Configuration

| Peripheral | Settings |
|---|---|
| USART2 | 115200 baud, 8N1, interrupt mode |
| ADC1 | 12-bit, single conversion, software trigger, interrupt |
| GPIO PA5 | Output push-pull (LED) |
| GPIO PC13 | Input (button) |

## Project Structure

```
Core/
└── Src/
    └── main.c    # Main application logic
```

## Getting Started

1. Open in **STM32CubeIDE**
2. Connect Nucleo board via USB
3. Build and flash (`Run → Debug`)
4. Open serial terminal at **115200 baud**
5. Send characters `0`–`5` to control the LED