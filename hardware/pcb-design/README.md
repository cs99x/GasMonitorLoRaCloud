
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

## Component Details

| **Reference**       | **Description**             | **Footprint**                                      | **Qty** |
|----------------------|-----------------------------|---------------------------------------------------|---------|
| C1, C7, C8, C12      | 10uF Capacitor             | Capacitor_SMD:C_0603_1608Metric                  | 4       |
| C2                  | 220nF Capacitor            | Capacitor_SMD:C_0603_1608Metric                  | 1       |
| C3                  | 1uF Capacitor              | Capacitor_SMD:C_0603_1608Metric                  | 1       |
| D1, D3              | WS2812B RGB LEDs           | LED_SMD:LED_WS2812B_PLCC4_5.0x5.0mm_P3.2mm       | 2       |
| J2                  | USB-C Receptacle           | Connector_USB:USB_C_Receptacle_GCT_USB4085       | 1       |
| U6                  | ESP32-S3-WROOM-1 Module    | RF_Module:ESP32-S3-WROOM-1                       | 1       |
| U8, U9, U10, U11    | MQ-4 Gas Sensors           | Sensor:MQ-6                                      | 4       |

**For the complete Bill of Materials (BOM), refer to the project documentation.**

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

## Upcoming Features

1. **Firmware Updates**:
   - Enhanced BLE and LoRa firmware for secure communication.
   - Advanced sensor data processing and analytics.
2. **User Interface Optimization**:
   - Improved RGB LED indications for system states.
   - Additional button-based interactions.
3. **Battery Management**:
   - Display battery percentage via firmware integration.
   - Enhanced Li-ion battery protection circuitry.

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

---

**Author/Team**: [Christoph Schmid/LoRa-Gasmeter]  

This PCB serves as the foundation for future IoT projects, providing a versatile and compact design for sensor-based applications.
