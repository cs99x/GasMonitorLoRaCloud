
# Gas Sensor Monitoring System

This firmware was build arround the ESP32 S3 microcontroller. It utilizes multiple MQ-series sensors to measure various gas concentrations and display them in real-time on a OLED display. The firmware is written in C++ using the Arduino framework. 

---

## Features

- **Gas Detection**:
  - Supports MQ-series sensors:
    - CH4 (Methane)
    - CO2 (Carbon dioxide)
    - LPG (Liquefied petroleum gas)
    - CO (Carbon monoxide)
  - Provides real-time measurements in parts per million (PPM).

- **User Interface**:
  - OLED Display (Adafruit SH110X) for clear visualization of gas levels and system status.
  - Enhanced **User Interaction**:
    - Button-based navigation to toggle between multiple screens (e.g., gas levels, battery status).
    - Intuitive layout with bar graphs and data labels for easy readability.
  
- **Alerts**:
  - Buzzer alerts when gas levels exceed configurable thresholds.

- **Debugging Tools**:
  - Serial monitoring of JSON-formatted sensor data for easy troubleshooting.
  - Logging mechanisms to capture button states and alert conditions.

---

## Hardware Requirements

1. **ESP32 S3 Development Board**
2. **Sensors**:
   - CH4
   - CO2
   - LPG
   - CO
3. **OLED Display**: Adafruit SH110X (128x64 resolution)
4. **Buzzer**: Active buzzer for alerts
5. **Button**: For UI navigation
7. **Power Source**: 5V supply for ESP32 and sensors like a powerbank

---

## Software Requirements

- **PlatformIO** or **Arduino IDE** for development
- Required Libraries:
  - **Adafruit GFX**
  - **Adafruit SH110X**
  - **ArduinoJson**
  - **FreeRTOS**
  - **Wire**
  - **SPI**
  - **LoRa**

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
  - Sensor values in ppm
  - State of device
- **Navigation**:
  - Button-press toggles between screens.
  - Alerts for threshold violations.

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
   - Gas concentrations with bar graphs.
  - Battery percentage and system status.
  - LoRa RSSI and SNR.
3. **Advanced Debugging**:
   - In-depth error logging.
   - On-screen debugging information.
4. **Battery Optimization**:
   - Integration with a Li-ion battery and charge monitoring.
   - Display charging status and runtime predictions.
5. **LoRa Stack**:
   - Usage of the LoRa Stack.
   - Including of ACK (duplex communication)

---

**License**: MIT License

---
