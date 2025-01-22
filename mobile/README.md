
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

## Future Enhancements

- Add support for writing commands to BLE devices.
- Include historical data storage and visualization.
- Implement error handling for connection and JSON parsing issues.
- Implement buffer if iOS device isnt conencted to the web.
- Improve UI for better user experience.

## License

This project is open-source and licensed under the MIT License.
