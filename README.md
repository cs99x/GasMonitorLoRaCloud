
# GasMonitorLoRaCloud

The **GasMonitorLoRaCloud** project is a IoT solution designed to monitor gas levels via a handheld device using LoRa communication, integrate with cloud platforms for data storage and visualization, and provide mobile app support for real-time monitoring.

## Features

- **Gas Level Monitoring**: Continuously measures four different gas concentrations.
- **LoRa Communication**: Transmits data over long distances using LoRa technology.
- **LoRa Gateway**: Receives data sent by the handheld device via LoRa and transmits them via bluetooth to the mobile app.
- **iOS Mobile App**: Allows the user to connect to the LoRa Gateway and display real time sensor values and transmits data to the cloud.
- **Cloud Integration**: Contains a database with sensor data and hosts a website.
- **Data Visualization**: Offers real-time and historical data visualization through cloud dashboards.

---

## Components

### 1. Hardware handheld device
Responsible for measuring and displaying real time sensor values. Transmission of the data is via LoRa and gets received from the LoRa Bluetooth gateway.

- **Gas Sensor**: Detects specific gas concentrations using MQx sensors.
- **LoRa Module**: Manages long-range wireless communication.
- **Microcontroller**: Interfaces with the sensor and LoRa module.
- **OLED Display**: Displays current sensor values as well as device state.
- **Power Supply**: Li-Ion battery which gets charged via USB Type C Port (upcoming).

For detailed information on hardware setup, refer to the respective READMEs:

- [Handheld Gas Meter Firmware](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/firmware/handheld-gasmeter/README.md)
- [PCB Design](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/pcb-design/README.md)
- [CAD Designs](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/cad/README.md)

### 2. Hardware LoRa Bluetooth Gateway
This devices acts as a gateway between the handheld device and the mobile app/cloud backend. It receives data via LoRa and transmits them via bluetooth to a mobile app. 

- **LoRa Module**: Manages long-range wireless communication.
- **Microcontroller**: Interfaces with the LoRa module and the Bluetooth.
- **OLED Display**: Displays current sensor values as well as device state (upcoming).
- **Power Supply**: Li-Ion battery which gets charged via USB Type C Port (upcoming).

For detailed information on hardware setup, refer to the respective READMEs:
- [LoRa-Bluetooth Gateway Firmware](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/firmware/lora-bluetooth-gateway/README.md)
- [PCB Design](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/pcb-design/README.md)
- [CAD Designs](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/hardware/cad/README.md)

### 3. iOS App

The iOS app displays in realtime the sensor data received from the handheld device. Its connected with the LoRa Gateway via BLE and transmits the data to a cloud backend for further processing.

#### Key Features

- **Bluetooth Scanning**: Discover nearby Bluetooth Low Energy (BLE) devices.
- **Device Connection**: Connect to selected devices and subscribe to their data notifications.
- **Real-time Sensor Data**: Display detailed information such as battery status, sensor readings, temperature, and alarm statuses.
- **JSON Parsing**: Receive and parse JSON data from devices for structured display.
- **Cloud Transmitting**: Transmits the data to a cloud backend for further processing.

For detailed setup instructions and usage, refer to the [Mobile README](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/mobile/README.md).

### 4. Cloud Platform

- **Data Storage**: Collects and stores gas level data received from the LoRa gateway.
- **Visualization Tools**: Provides dashboards for monitoring real-time and historical data.
- **Alert Mechanisms**: Notifies users when gas levels exceed predefined thresholds (upcoming).

For more details on the backend setup, see the [Backend README](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/backend/README.md).

---

## Future Enhancements

- Designing of SLS printed case
- Completing of custom pcb for handheld as well as LoRa Gateway
- Support for additional gas sensors.
- Calibrations mode for gas sensors.
- Improved UI for handheld device.
- Adding OLED display on LoRa Gateway for sensor data and transmission state.
- Improved mobile app UI/UX.
- Development of an Android app.
- Advanced cloud-based analytics and reporting.

---

## License

This project is open-source and licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
