# GasMonitorLoRaCloud

## Overview

This project contains the firmware for a handheld gas meter and a LoRa Bluetooth gateway. The gas meter is designed to detect and measure gas concentrations, while the gateway facilitates communication between the gas meter and the cloud. Both are programmed with the Arduino framework.

## Features

### Handheld Gas Meter
- Real-time gas concentration measurement
- Portable and easy to use
- OLED display for gas readings

### LoRa Bluetooth Gateway
- Long-range communication using LoRa technology
- Bluetooth connectivity for local data transfer

## Getting Started

### Prerequisites
- Handheld gas meter device
- LoRa Bluetooth gateway device
- Compatible power sources
- Mobile ios device with Bluetooth capability
- Internet connection for cloud access via the ios app

### Installation

1. **Handheld Gas Meter**
    - Connect device with a powerbank (Li-ion battery usage planned later on).
    - Power on the device using the power button.

2. **LoRa Bluetooth Gateway**
    - Connect the gateway to a power source eg. powerbank or ios device.
    - Ensure the gateway is within range of the gas meter.
    - Pair the gateway with your mobile device via Bluetooth.

### Usage

1. **Measuring Gas Concentration**
    - Turn on the handheld gas meter.
    - Place the sensor in the area to be measured.
    - Read the gas concentration on the OLED display.

2. **Data Transmission**
    - Ensure the gas meter is within range of the LoRa Bluetooth gateway.
    - The gateway will automatically transmit data to the ios app.
    - Use the mobile app to monitor gas levels remotely.

- **Gateway Not Connecting**
  - Verify Bluetooth is enabled on your mobile device.
  - If LoRa or bluetooth transmission isnt established, restart both devices.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
