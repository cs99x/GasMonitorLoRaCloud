import CoreBluetooth
import SwiftUI

class BluetoothManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    @Published var discoveredPeripherals: [(peripheral: CBPeripheral, name: String)] = [] // List of discovered devices
    @Published var isConnected = false
    @Published var sensorValue: String = "No data yet"
    @Published var deviceData: DeviceData? // Add this property to store parsed JSON data

    private var centralManager: CBCentralManager!
    private var sensorPeripheral: CBPeripheral?
    private let serviceUUID = CBUUID(string: "12345678-1234-1234-1234-123456789abc")
    private let characteristicUUID = CBUUID(string: "87654321-4321-4321-4321-abc123456789")

    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }

    // MARK: - Start Scanning
    func startScan() {
        discoveredPeripherals = [] // Clear previous scan results
        centralManager.scanForPeripherals(withServices: nil, options: nil)
        print("Scanning for peripherals...")

        // Stop scanning after 10 seconds
        DispatchQueue.main.asyncAfter(deadline: .now() + 10) {
            self.centralManager.stopScan()
            print("Stopped scanning.")
        }
    }

    // MARK: - Central Manager Delegate
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            print("Bluetooth is powered on, ready to scan.")
        } else {
            print("Bluetooth is not available.")
        }
    }

    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String: Any], rssi RSSI: NSNumber) {
        let deviceName = peripheral.name ?? "Unknown Device"
        print("Discovered peripheral: \(deviceName)")

        // Check if the device is already in the list
        if !discoveredPeripherals.contains(where: { $0.peripheral.identifier == peripheral.identifier }) {
            DispatchQueue.main.async {
                self.discoveredPeripherals.append((peripheral, deviceName))
            }
            print("Added \(deviceName) to the list.")
        } else {
            print("\(deviceName) is already in the list.")
        }
    }


    func connectToPeripheral(_ peripheral: CBPeripheral) {
        print("Connecting to peripheral: \(peripheral.name ?? "Unknown Device")")
        sensorPeripheral = peripheral
        sensorPeripheral?.delegate = self
        centralManager.connect(peripheral, options: nil)
    }

    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected to peripheral: \(peripheral.name ?? "Unknown")")
        isConnected = true
        peripheral.discoverServices([serviceUUID])
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        for service in services {
            print("Discovered service: \(service.uuid)")
            peripheral.discoverCharacteristics([characteristicUUID], for: service)
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        for characteristic in characteristics {
            if characteristic.uuid == characteristicUUID {
                print("Subscribing to characteristic notifications.")
                peripheral.setNotifyValue(true, for: characteristic)
            }
        }
    }

    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        guard let data = characteristic.value else {
            print("No data received.")
            return
        }

        if let jsonString = String(data: data, encoding: .utf8) {
            print("Received JSON: \(jsonString)")

            // Attempt to parse the JSON
            if let jsonData = jsonString.data(using: .utf8) {
                do {
                    let parsedData = try JSONDecoder().decode(DeviceData.self, from: jsonData)
                    DispatchQueue.main.async {
                        self.deviceData = parsedData
                    }
                    print("Parsed data successfully: \(parsedData)")
                } catch {
                    print("Failed to parse JSON: \(error)")
                }
            } else {
                print("Failed to convert JSON string to data.")
            }
        } else {
            print("Failed to decode data as UTF-8 string.")
        }
    }
}
