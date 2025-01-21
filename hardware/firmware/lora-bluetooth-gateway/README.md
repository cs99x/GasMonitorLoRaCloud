# ESP32 LoRa Bluetooth Gateway

This project implements a gateway that bridges LoRa and Bluetooth Low Energy (BLE) using the ESP32 microcontroller. It allows the ESP32 to receive JSON payloads over LoRa and send them via BLE notifications to a connected BLE client.

## Features
- **LoRa Communication**: Configured to receive JSON payloads on a specified frequency.
- **BLE Notifications**: Sends received payloads to a BLE-connected device.
- **FreeRTOS Multitasking**: Separates LoRa reception and BLE notification tasks to ensure smooth operation.
- **Data Synchronization**: Uses a semaphore for thread-safe data handling between tasks.

---

## Hardware Setup

### Components
1. ESP32 (e.g., DevKit v1)
2. LoRa Module (e.g., SX1278 or SX1276)
3. Jumper wires for connections

### Pin Connections
| LoRa Pin    | ESP32 Pin   |
|-------------|-------------|
| `SCK`       | GPIO 18     |
| `MISO`      | GPIO 19     |
| `MOSI`      | GPIO 23     |
| `SS`        | GPIO 5      |
| `RST`       | GPIO 14     |
| `DIO0`      | GPIO 26     |

Ensure that the LoRa module and ESP32 share a common ground.

---

## Software Requirements

### Dependencies
Install the following libraries in your Arduino IDE:
1. **BLE Libraries**
   - `BLEDevice.h`
   - `BLEUtils.h`
   - `BLEServer.h`
   - `BLE2902.h`
2. **LoRa Library**
   - `LoRa.h`
3. **ArduinoJson Library**
   - `ArduinoJson.h`

To install these, go to **Tools > Manage Libraries** in the Arduino IDE.

### FreeRTOS
FreeRTOS is natively supported on the ESP32 platform and is used for multitasking.

---

## How It Works

### 1. BLE Server
- The ESP32 acts as a BLE server.
- Upon connection, the client receives real-time JSON payloads via BLE notifications.

### 2. LoRa Receiver
- LoRa listens for incoming JSON payloads.
- Upon receiving data, it is parsed and stored in a shared buffer.

### 3. Multitasking with FreeRTOS
- **BLE Task**: Sends data from the shared buffer to the connected BLE client.
- **LoRa Task**: Continuously listens for LoRa packets and updates the shared buffer.

---

## Configuration

### LoRa
- Frequency: `915 MHz`
- Sync Word: `0x34`
- Adjust frequency as per your LoRa module and regional regulations.

### BLE
- Service UUID: `12345678-1234-1234-1234-123456789abc`
- Characteristic UUID: `87654321-4321-4321-4321-abc123456789`

### Task Priorities
- BLE Task Priority: `1`
- LoRa Task Priority: `2`

---

## Usage

1. **Upload the Code**
   - Flash the provided code onto the ESP32 using the Arduino IDE.

2. **Power the System**
   - Power the ESP32 and ensure the LoRa module is properly connected.

3. **Connect to BLE**
   - Use any BLE client app (e.g., nRF Connect) to connect to the ESP32.
   - Subscribe to notifications for the characteristic UUID.

4. **Send Data via LoRa**
   - Transmit JSON payloads to the ESP32 using another LoRa device.

5. **Receive Notifications**
   - View the received JSON data on your BLE client app.

---

## Debugging

- Use the Serial Monitor (115200 baud rate) to view logs:
  - BLE connection status.
  - LoRa reception details.
  - JSON payloads sent via BLE.

---

## Example Output

**Serial Monitor:**
BLE advertising started. Device connected! Starting LoRa reception... LoRa packet received. Complete JSON payload received: {"temperature": 25.4, "humidity": 60.1} JSON payload sent via BLE: {"temperature": 25.4, "humidity": 60.1}

**BLE Client Notification:**
{"temperature": 25.4, "humidity": 60.1}


---

## Future Enhancements
- Add support for sending commands from BLE to LoRa.
- Implement encryption for LoRa and BLE communication.
- Expand to handle multiple BLE clients simultaneously.

---

## License
This project is open-source and available under the MIT License. Feel free to modify and distribute.

---

## Credits
Developed using the Arduino framework and inspired by IoT gateway architectures.

This README.md provides a comprehensive overview of your project, detailing its features, setup, usage, and future development possibilities.