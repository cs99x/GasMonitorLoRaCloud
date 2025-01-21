
# GasMonitorLoRaCloud

The **GasMonitorLoRaCloud** project is a complete IoT solution designed to monitor gas levels using LoRa communication, integrate with cloud platforms for data storage and visualization, and provide mobile app support for real-time monitoring.

## Features

- **Gas Level Monitoring**: Continuously measures gas concentrations using sensors.
- **LoRa Communication**: Transmits data over long distances using LoRa technology.
- **Cloud Integration**: Sends collected data to cloud platforms for storage and analysis.
- **Data Visualization**: Provides real-time and historical data visualization through cloud dashboards.
- **iOS Mobile App**: A SwiftUI-based app to monitor gas levels in real-time and receive notifications directly on your mobile device.

---

## Components

### 1. Hardware
- **Gas Sensor**: Detects specific gas concentrations.
- **LoRa Module**: Handles long-range wireless communication.
- **Microcontroller**: Interfaces with the sensor and LoRa module.
- **Power Supply**: Powers the hardware components.

### 2. Cloud Platform
- Stores gas level data received from the LoRa gateway.
- Provides visualization tools and alert mechanisms for critical thresholds.

### 3. iOS App
The iOS app enhances the project by providing real-time monitoring and interaction with gas sensors via Bluetooth. It also integrates with the cloud for remote data visualization.

#### Features
- **Bluetooth Scanning**: Discover nearby Bluetooth Low Energy (BLE) devices.
- **Device Connection**: Connect to selected devices and subscribe to their data notifications.
- **Real-time Sensor Data**: Display information like:
  - Battery status
  - Sensor readings
  - Temperature
  - Alarm and fault statuses
- **JSON Parsing**: Receives and parses JSON data from devices for structured display.

#### JSON Data Structure
The app expects device data in the following JSON format:
```json
{
  "id": "DeviceID123",
  "ts": "2025-01-21T12:34:56Z",
  "batt": {
    "pct": 80,
    "chg": true
  },
  "sensors": [
    {
      "type": "temperature",
      "name": "Ambient Temp",
      "unit": "C",
      "val": 22.5
    }
  ],
  "temp": {
    "type": "temperature",
    "unit": "C",
    "val": 22.5
  },
  "stat": {
    "alarm": false,
    "fault": false
  }
}
```

---

## Setup Instructions

### Hardware Setup
1. **Hardware Assembly**:
   - Connect the gas sensor to the microcontroller.
   - Interface the LoRa module with the microcontroller.
   - Ensure proper power connections.

2. **Configuration**:
   - Flash the microcontroller with the appropriate firmware using the Arduino IDE.
   - Configure LoRa parameters such as frequency and sync word.

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

### Cloud Platform Setup
1. **Platform Setup**:
   - Create an account on your preferred IoT platform (e.g., AWS IoT, Google Cloud IoT, or a custom solution).
   - Set up data ingestion endpoints for the LoRa gateway to send sensor data.

2. **Dashboard Setup**:
   - Design real-time dashboards to monitor gas levels.
   - Configure alert rules to notify users of abnormal gas levels.

---

## Features Overview

| Component      | Features                                                                 |
|----------------|-------------------------------------------------------------------------|
| **Hardware**   | Gas level monitoring, LoRa transmission.                               |
| **Cloud**      | Data storage, visualization, and alerting.                             |
| **iOS App**    | Real-time monitoring, BLE connection, cloud integration, and alerts.   |

---

## Example Usage

### iOS App
- **Connected Device**:
  ```
  Device: Device123
  Timestamp: 2025-01-21T12:34:56Z
  Battery: 80%
  Charging: Yes
  Sensors:
    - Ambient Temp (temperature): 22.5 C
  Alarm Triggered: No
  Sensor Fault: No
  Temperature: 22.5 C
  ```

### Console Log (iOS App)
```
Scanning for peripherals...
Discovered peripheral: SensorDevice1
Added SensorDevice1 to the list.
Connecting to peripheral: SensorDevice1
Connected to peripheral: SensorDevice1
Received JSON: {"id":"Device123",...}
Parsed data successfully: DeviceData(id: "Device123", ...)
```

---

## Future Enhancements

- Support for additional gas sensors.
- Improved mobile app as well as web UI/UX.
- Android app development.
- Advanced cloud-based analytics and reporting.
- Integration with machine learning models for predictive analysis.

---

## License

This project is open-source and licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
