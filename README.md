
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

## Future Enhancements

- Support for additional gas sensors.
- Improved mobile app UI/UX.
- Development of an Android app.
- Advanced cloud-based analytics and reporting.
- Integration with machine learning models for predictive analysis.

---

## License

This project is open-source and licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
