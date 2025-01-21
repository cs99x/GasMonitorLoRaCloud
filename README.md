
# GasMonitorLoRaCloud

The **GasMonitorLoRaCloud** project is a comprehensive IoT solution designed to monitor gas levels using LoRa communication, integrate with cloud platforms for data storage and visualization, and provide mobile app support for real-time monitoring.

## Features

- **Gas Level Monitoring**: Continuously measures gas concentrations using sensors.
- **LoRa Communication**: Transmits data over long distances using LoRa technology.
- **Cloud Integration**: Sends collected data to cloud platforms for storage and analysis.
- **Data Visualization**: Offers real-time and historical data visualization through cloud dashboards.
- **iOS Mobile App**: Allows users to scan, connect, and interact with Bluetooth-enabled devices, providing real-time sensor data in a user-friendly interface.

---

## Components

### 1. Hardware

- **Gas Sensor**: Detects specific gas concentrations.
- **LoRa Module**: Manages long-range wireless communication.
- **Microcontroller**: Interfaces with the sensor and LoRa module.
- **Power Supply**: Powers the hardware components.

For detailed information on hardware setup, refer to the respective READMEs:

- [CAD Designs](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/cad/README.md)
- [Handheld Gas Meter Firmware](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/firmware/handheld-gasmeter/README.md)
- [LoRa-Bluetooth Gateway Firmware](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/firmware/lora-bluetooth-gateway/README.md)
- [PCB Design](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/pcb-design/README.md)

### 2. Cloud Platform

- **Data Storage**: Collects and stores gas level data received from the LoRa gateway.
- **Visualization Tools**: Provides dashboards for monitoring real-time and historical data.
- **Alert Mechanisms**: Notifies users when gas levels exceed predefined thresholds.

For more details on the backend setup, see the [Backend README](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/backend/README.md).

### 3. iOS App

The iOS app enhances the project by providing real-time monitoring and interaction with gas sensors via Bluetooth.

#### Key Features

- **Bluetooth Scanning**: Discover nearby Bluetooth Low Energy (BLE) devices.
- **Device Connection**: Connect to selected devices and subscribe to their data notifications.
- **Real-time Sensor Data**: Display detailed information such as battery status, sensor readings, temperature, and alarm statuses.
- **JSON Parsing**: Receive and parse JSON data from devices for structured display.

For detailed setup instructions and usage, refer to the [Mobile README](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/mobile/README.md).

---

## Setup Instructions

### Hardware Setup

1. **Assembly**:
   - Connect the gas sensor to the microcontroller.
   - Interface the LoRa module with the microcontroller.
   - Ensure proper power connections.

2. **Configuration**:
   - Flash the microcontroller with the appropriate firmware.
   - Configure LoRa parameters such as frequency and sync word.

### Cloud Platform Setup

1. **Platform Setup**:
   - Create an account on your preferred IoT platform (e.g., AWS IoT, Google Cloud IoT).
   - Set up data ingestion endpoints for the LoRa gateway to send sensor data.

2. **Dashboard Setup**:
   - Design real-time dashboards to monitor gas levels.
   - Configure alert rules to notify users of abnormal gas levels.

### iOS App Setup

1. **Environment Setup**:
   - Install Xcode on macOS.
   - Ensure Bluetooth is enabled on your Mac and test device.

2. **Clone the Repository**:
   ```bash
   git clone https://github.com/cs99x/GasMonitorLoRaCloud.git
   cd GasMonitorLoRaCloud/mobile
   ```

3. **Build and Run**:
   - Open the project in Xcode.
   - Select a device or simulator to run the app.
   - Build and run the app.

4. **Test with a BLE Device**:
   - Ensure a BLE device is available that sends data in the expected JSON format.
   - Start scanning, connect to the device, and view the sensor data.

---

## Future Enhancements

- Support for additional gas sensors.
- Improved mobile app UI/UX.
- Development of an Android app.
- Advanced cloud-based analytics and reporting.
- Integration with machine learning models for predictive analysis.

---

## License

This project is open-source and licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
