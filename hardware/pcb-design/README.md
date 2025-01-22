
# 2-Layer PCB for ESP32-Based IoT Projects

This project includes a 2-layer PCB design (1.6mm thickness) for an ESP32-based IoT application. The PCB integrates BLE, LoRa, and gas sensors, and is designed to handle power management, sensor interfacing, and user interaction efficiently. This board is intended to be used in upcoming projects with additional hardware and software features.

---

## Key Features

- **ESP32-S3-WROOM-1 Module**: Provides Wi-Fi and BLE capabilities.
- **LoRa Communication Module**: Supports long-range wireless communication.
- **Gas Sensors (MQ Series)**: Interfaces with up to four MQ gas sensors.
- **Power Management**:
  - Integrated **TP4054** Li-ion battery charger.
  - **TPS63020** buck-boost converters for stable power supply.
  - Battery connector and monitoring.
- **User Interaction**:
  - Multiple tactile buttons for input.
  - Integrated WS2812B RGB LEDs for status indication.
- **Peripheral Connections**:
  - USB-C for power and communication.
  - JST connector for Li-ion battery.
- **Compact Design**:
  - 1.6mm thickness, 2-layer PCB.
  - Mounting holes for easy integration into enclosures.

---

## Electrical Specifications

- **Power Input**:
  - USB-C: 5V
  - Li-ion Battery (via JST connector): 3.7V
- **Power Outputs**:
  - 3.3V and 5V rails powered by TPS63020 converters.
- **Current Handling**:
  - 6V 1.2A fuse for overcurrent protection.

---

## PCB Dimensions and Design

- **Layers**: 2
- **Thickness**: 1.6mm
- **Mounting**: Includes 4 mounting holes for M2 screws.
- **Keep-Out Zone**: Clearly marked for sensitive components.

---

## Usage Instructions

1. **Powering the Board**:
   - Connect a USB-C cable for 5V input or use a 3.7V Li-ion battery via the JST connector.
2. **Connecting Sensors**:
   - Plug MQ-series sensors into the designated headers.
3. **Programming**:
   - Use the USB-C port to flash firmware using ESP32 tools.
4. **Testing**:
   - Observe RGB LEDs and Serial Monitor for system status and debug messages.

---

## License

This project is licensed under the MIT License.
