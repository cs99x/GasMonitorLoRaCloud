
# Bluetooth Sensor Monitor App

This SwiftUI-based app allows users to scan, connect, and interact with Bluetooth-enabled devices. It provides real-time sensor data received from connected devices in a user-friendly interface.

## Features

- **Bluetooth Scanning**: Discover nearby Bluetooth Low Energy (BLE) devices.
- **Device Connection**: Connect to selected devices and subscribe to their data notifications.
- **Real-time Sensor Data**: Display detailed information such as:
  - Battery status
  - Sensor readings
  - Temperature
  - Alarm and fault statuses
- **JSON Parsing**: Receive and parse JSON data from the device to extract sensor readings and statuses.

## How It Works

### Workflow
1. **Scan for Devices**:
   - The app scans for nearby BLE devices and displays a list of discovered peripherals.
2. **Connect to a Device**:
   - Users can select a device from the list to establish a connection.
3. **Receive and Display Data**:
   - Once connected, the app listens for incoming JSON data and displays it in a structured format.

### JSON Structure
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

## Key Components

### `ContentView`
- Manages the user interface for scanning, connecting, and displaying device data.
- Displays either a list of discovered devices or the details of the connected device.

### `BluetoothManager`
- Handles all Bluetooth-related operations, including:
  - Scanning for devices.
  - Connecting to peripherals.
  - Subscribing to characteristics for data updates.
- Parses incoming JSON data into the `DeviceData` model for structured display.

### `DeviceData`
- A Swift structure to decode and store sensor data received from the device.

## Setup Instructions

1. **Environment Setup**:
   - Install Xcode on macOS.
   - Ensure Bluetooth is enabled on your Mac and test device.

2. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd <repository-folder>
   ```

3. **Build and Run**:
   - Open the project in Xcode.
   - Select a device or simulator to run the app.
   - Build and run the app.

4. **Test with a BLE Device**:
   - Ensure a BLE device is available that sends data in the expected JSON format.
   - Start scanning, connect to the device, and view the sensor data.

## Screens

1. **Scanning Screen**:
   - Displays a list of discovered Bluetooth devices.
   - Allows users to connect to a device.

2. **Device Data Screen**:
   - Shows real-time sensor readings from the connected device.
   - Displays detailed battery status, sensor readings, temperature, and system status.

## Dependencies

- **CoreBluetooth**: Provides BLE functionality for scanning and connecting to devices.
- **SwiftUI**: Used for creating the app's user interface.

## Example Output

### Console Log
```
Scanning for peripherals...
Discovered peripheral: SensorDevice1
Added SensorDevice1 to the list.
Connecting to peripheral: SensorDevice1
Connected to peripheral: SensorDevice1
Received JSON: {"id":"Device123",...}
Parsed data successfully: DeviceData(id: "Device123", ...)
```

### App UI
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

## Future Enhancements

- Add support for writing commands to BLE devices.
- Include historical data storage and visualization.
- Implement error handling for connection and JSON parsing issues.
- Improve UI for better user experience.

## License

This project is open-source and licensed under the MIT License.
