
# GasMonitorLoRaCloud

The **GasMonitorLoRaCloud** project is a comprehensive IoT solution designed to monitor gas levels using LoRa communication, integrate with cloud platforms for data storage and visualization, and provide mobile app support for real-time monitoring.

## Features

- **Gas Level Monitoring**: Continuously measures gas concentrations using sensors (CH4, CO, LPG, H2).
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
- **REST API**: Provides endpoints for receiving sensor data and querying historical records.
- **API Documentation**: Interactive Swagger/OpenAPI documentation for easy API testing and integration.
- **Visualization Tools**: Provides dashboards for monitoring real-time and historical data.
- **Alert Mechanisms**: Notifies users when gas levels exceed predefined thresholds.

For more details on the backend setup, see the [Backend README](https://github.com/cs99x/GasMonitorLoRaCloud/blob/main/backend/README.md).

#### API Endpoints

The backend provides a REST API with the following endpoints:

- **POST `/receive-json`**: Submit sensor data from IoT devices
- **GET `/get-last-entity`**: Retrieve the most recent sensor reading
- **GET `/get-data-from-timestamp`**: Query historical sensor data from a specific timestamp

**Interactive API Documentation**: Access the Swagger UI at `http://<host>:8000/apidocs/` to test the API directly from your browser.

#### API Usage Examples

The Gas Monitor API supports four sensor types: **CH4 (Methane)**, **CO (Carbon Monoxide)**, **LPG (Liquefied Petroleum Gas)**, and **H2 (Hydrogen)**. Below are examples for uploading and retrieving sensor data.

##### Uploading Sensor Data

Use the `POST /receive-json` endpoint to submit sensor readings. Each request can include multiple sensor types in a single payload.

**Example: Uploading CH4 (Methane) Sensor Data**

```bash
curl -X POST http://localhost:8000/receive-json \
  -H "Content-Type: application/json" \
  -d '{
    "pct": 85,
    "id": "sensor-device-1",
    "ts": 1703174400,
    "sensors": [
      {"type": "CH4", "val": 450}
    ]
  }'
```

**Example: Uploading CO (Carbon Monoxide) Sensor Data**

```bash
curl -X POST http://localhost:8000/receive-json \
  -H "Content-Type: application/json" \
  -d '{
    "pct": 90,
    "id": "sensor-device-2",
    "ts": 1703174500,
    "sensors": [
      {"type": "CO", "val": 35}
    ]
  }'
```

**Example: Uploading LPG (Liquefied Petroleum Gas) Sensor Data**

```bash
curl -X POST http://localhost:8000/receive-json \
  -H "Content-Type: application/json" \
  -d '{
    "pct": 78,
    "id": "sensor-device-3",
    "ts": 1703174600,
    "sensors": [
      {"type": "LPG", "val": 120}
    ]
  }'
```

**Example: Uploading H2 (Hydrogen) Sensor Data**

```bash
curl -X POST http://localhost:8000/receive-json \
  -H "Content-Type: application/json" \
  -d '{
    "pct": 92,
    "id": "sensor-device-4",
    "ts": 1703174700,
    "sensors": [
      {"type": "H2", "val": 280}
    ]
  }'
```

**Example: Uploading Multiple Sensor Types Together**

```bash
curl -X POST http://localhost:8000/receive-json \
  -H "Content-Type: application/json" \
  -d '{
    "pct": 88,
    "id": "handheld-meter-1",
    "ts": 1703174800,
    "sensors": [
      {"type": "CH4", "val": 450},
      {"type": "CO", "val": 35},
      {"type": "LPG", "val": 120},
      {"type": "H2", "val": 280}
    ]
  }'
```

**Response Example:**

```json
{
  "received": {
    "pct": 88,
    "id": "handheld-meter-1",
    "ts": 1703174800,
    "sensors": [
      {"type": "CH4", "val": 450},
      {"type": "CO", "val": 35},
      {"type": "LPG", "val": 120},
      {"type": "H2", "val": 280}
    ]
  },
  "system_time": 1703174805
}
```

##### Retrieving Sensor Data

**Get Latest Sensor Reading**

Use `GET /get-last-entity` to retrieve the most recent sensor data entry:

```bash
curl -X GET http://localhost:8000/get-last-entity
```

**Response Example:**

```json
{
  "pct": 88,
  "device_id": "handheld-meter-1",
  "system_time": 1703174805,
  "system_datetime": "2023-12-21T15:20:05",
  "sensors": [
    {"type": "CH4", "val": 450},
    {"type": "CO", "val": 35},
    {"type": "LPG", "val": 120},
    {"type": "H2", "val": 280}
  ]
}
```

**Get Historical Sensor Data**

Use `GET /get-data-from-timestamp` to retrieve sensor data from a specific timestamp onwards:

```bash
curl -X GET "http://localhost:8000/get-data-from-timestamp?system_time=1703174400"
```

**Response Example:**

```json
[
  {
    "pct": 85,
    "device_id": "sensor-device-1",
    "system_time": 1703174405,
    "system_datetime": "2023-12-21T15:13:25",
    "sensors": [
      {"type": "CH4", "val": 450}
    ]
  },
  {
    "pct": 90,
    "device_id": "sensor-device-2",
    "system_time": 1703174505,
    "system_datetime": "2023-12-21T15:15:05",
    "sensors": [
      {"type": "CO", "val": 35}
    ]
  },
  {
    "pct": 78,
    "device_id": "sensor-device-3",
    "system_time": 1703174605,
    "system_datetime": "2023-12-21T15:16:45",
    "sensors": [
      {"type": "LPG", "val": 120}
    ]
  },
  {
    "pct": 92,
    "device_id": "sensor-device-4",
    "system_time": 1703174705,
    "system_datetime": "2023-12-21T15:18:25",
    "sensors": [
      {"type": "H2", "val": 280}
    ]
  }
]
```

### 3. iOS App

The iOS app enhances the project by providing real-time monitoring and interaction with gas sensors via Bluetooth and establishing a conectiontion to the cloud.

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
