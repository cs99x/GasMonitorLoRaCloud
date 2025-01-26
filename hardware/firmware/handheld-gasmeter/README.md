
# Gas Sensor Monitoring System

This project is a gas sensor monitoring system built around the ESP32 microcontroller. It utilizes multiple MQ-series sensors to measure various gas concentrations and display them in real-time. The system is designed for modularity and future enhancements, including **FreeRTOS multithreading**, optimized user interaction, debugging tools, and improved battery support.

---

## Features

- **Gas Detection**:
  - Supports MQ-series sensors:
    - MQ3 (Alcohol/Ethanol)
    - MQ135 (Air Quality/CO2)
    - MQ2 (Flammable Gas)
    - MQ9 (Carbon Monoxide)
  - Provides real-time measurements in parts per million (PPM).

- **User Interface**:
  - OLED Display (Adafruit SH110X) for clear visualization of gas levels and system status.
  - Enhanced **User Interaction**:
    - Button-based navigation to toggle between multiple screens (e.g., gas levels, battery status).
    - Intuitive layout with bar graphs and data labels for easy readability.
  
- **Alerts**:
  - Buzzer alerts when gas levels exceed configurable thresholds.
  
- **Battery Monitoring**:
  - Tracks battery percentage and displays it on the OLED screen.
  - Designed for integration with Li-ion battery power sources.

- **FreeRTOS**:
  - Tasks for managing sensor readouts, display updates, and future LoRa integration.
  - Multithreading planned for smoother task management.

- **Debugging Tools**:
  - Serial monitoring of JSON-formatted sensor data for easy troubleshooting.
  - Logging mechanisms to capture button states and alert conditions.

---

## Hardware Requirements

1. **ESP32 Development Board**
2. **Sensors**:
   - MQ3
   - MQ135
   - MQ2
   - MQ9
3. **OLED Display**: Adafruit SH110X (128x64 resolution)
4. **Buzzer**: Active buzzer for alerts
5. **Button**: For UI navigation
6. **Battery**: Li-ion battery for portable operation
7. **Power Source**: 5V supply for ESP32 and sensors

---

## Software Requirements

- **PlatformIO** or **Arduino IDE** for development
- Required Libraries:
  - **Adafruit GFX**
  - **Adafruit SH110X**
  - **ArduinoJson**
  - **FreeRTOS**

---

## Functional Overview

### Gas Sensors
- Calculates gas concentrations using sensor-specific logarithmic equations.
- Example formula for PPM calculation:
  ```c
  float ppm = pow(10, (log10(ratioRsR0) - b) / m);
  ```
  - `Rs`: Sensor resistance
  - `R0`: Calibration resistance
  - `m` and `b`: Sensor-specific constants

### User Interface
- **Display Screens**:
  - Gas concentrations with bar graphs.
  - Battery percentage and system status.
  - LoRa connection data (planned).
- **Navigation**:
  - Button-press toggles between screens.
  - Alerts for threshold violations.

### FreeRTOS Tasks
- **Sensor Display Task** (Core 0):
  - Reads sensor data and updates the display.
  - Handles user input via the button.
- **LoRa Task** (Core 1):
  - Placeholder for future LoRa integration.
  - Simulates activity with blinking LEDs.

### Battery Monitoring
- Monitors and displays battery percentage on the OLED.
- Supports portable operation using Li-ion batteries.

### Debugging
- Serial output of JSON-formatted sensor data for external tools.
- Tracks system states (e.g., button presses, buzzer activations).

---

## Future Plans

1. **FreeRTOS Multithreading**:
   - Improved task separation for smoother operation.
   - Dedicated threads for data transmission and UI updates.
2. **Optimized UI**:
   - Enhanced screen layouts and user navigation.
   - Additional interaction options for system control.
3. **Advanced Debugging**:
   - In-depth error logging.
   - On-screen debugging information.
4. **Battery Optimization**:
   - Integration with a Li-ion battery and charge monitoring.
   - Display charging status and runtime predictions.
5. **LoRa Integration**:
   - Real-time data transmission to remote servers.

---

**License**: MIT License

---

This README provides an overview of the current features and roadmap for the gas monitoring system. With the planned enhancements, it aims to be a robust solution for real-time gas detection and monitoring.
