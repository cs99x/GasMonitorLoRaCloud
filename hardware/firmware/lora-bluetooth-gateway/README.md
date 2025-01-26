# LoRa Bluetooth Gateway

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
