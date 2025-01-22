import CoreBluetooth
import SwiftUI

/// Manages Bluetooth operations including scanning, connecting, and receiving data.
class BluetoothManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    /// List of discovered Bluetooth peripherals and their names.
    @Published var discoveredPeripherals: [(peripheral: CBPeripheral, name: String)] = []
    
    /// Indicates whether a connection to a peripheral is established.
    @Published var isConnected = false
    
    /// The received sensor value as a string.
    @Published var sensorValue: String = "No data yet"
    
    /// Parsed JSON data received from the connected peripheral.
    @Published var deviceData: DeviceData?
    
    /// The central manager responsible for managing Bluetooth operations.
    private var centralManager: CBCentralManager!
    
    /// The currently connected peripheral.
    private var sensorPeripheral: CBPeripheral?
    
    /// UUID for the desired Bluetooth service.
    private let serviceUUID = CBUUID(string: "12345678-1234-1234-1234-123456789abc")
    
    /// UUID for the characteristic to interact with.
    private let characteristicUUID = CBUUID(string: "87654321-4321-4321-4321-abc123456789")
    
    /// Initializes the Bluetooth manager and sets up the central manager delegate.
    override init() {
        super.init()
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    /// Starts scanning for Bluetooth peripherals.
    ///
    /// Clears previous scan results and scans for peripherals for 10 seconds.
    func startScan() {
        discoveredPeripherals = []
        centralManager.scanForPeripherals(withServices: nil, options: nil)
        print("Scanning for peripherals...")
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 10) {
            self.centralManager.stopScan()
            print("Stopped scanning.")
        }
    }
    
    /// Called when the state of the central manager changes.
    ///
    /// - Parameter central: The central manager instance.
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            print("Bluetooth is powered on, ready to scan.")
        } else {
            print("Bluetooth is not available.")
        }
    }
    
    /// Called when a peripheral is discovered during scanning.
    ///
    /// - Parameters:
    ///   - central: The central manager instance.
    ///   - peripheral: The discovered peripheral.
    ///   - advertisementData: Advertisement data from the peripheral.
    ///   - RSSI: The signal strength of the peripheral.
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String: Any], rssi RSSI: NSNumber) {
        let deviceName = peripheral.name ?? "Unknown Device"
        print("Discovered peripheral: \(deviceName)")
        
        if !discoveredPeripherals.contains(where: { $0.peripheral.identifier == peripheral.identifier }) {
            DispatchQueue.main.async {
                self.discoveredPeripherals.append((peripheral, deviceName))
            }
            print("Added \(deviceName) to the list.")
        }
    }
    
    /// Connects to the specified peripheral.
    ///
    /// - Parameter peripheral: The peripheral to connect to.
    func connectToPeripheral(_ peripheral: CBPeripheral) {
        print("Connecting to peripheral: \(peripheral.name ?? "Unknown Device")")
        sensorPeripheral = peripheral
        sensorPeripheral?.delegate = self
        centralManager.connect(peripheral, options: nil)
    }
    
    /// Called when a connection to a peripheral is established.
    ///
    /// - Parameters:
    ///   - central: The central manager instance.
    ///   - peripheral: The connected peripheral.
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected to peripheral: \(peripheral.name ?? "Unknown")")
        isConnected = true
        peripheral.discoverServices([serviceUUID])
    }
    
    /// Called when services are discovered on a peripheral.
    ///
    /// - Parameters:
    ///   - peripheral: The peripheral with discovered services.
    ///   - error: An error object if the discovery failed.
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard let services = peripheral.services else { return }
        for service in services {
            print("Discovered service: \(service.uuid)")
            peripheral.discoverCharacteristics([characteristicUUID], for: service)
        }
    }
    
    /// Called when characteristics are discovered on a service.
    ///
    /// - Parameters:
    ///   - peripheral: The peripheral with discovered characteristics.
    ///   - service: The service containing the characteristics.
    ///   - error: An error object if the discovery failed.
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        for characteristic in characteristics {
            if characteristic.uuid == characteristicUUID {
                print("Subscribing to characteristic notifications.")
                peripheral.setNotifyValue(true, for: characteristic)
            }
        }
    }
    
    /// Called when a characteristic value is updated.
    ///
    /// - Parameters:
    ///   - peripheral: The peripheral containing the characteristic.
    ///   - characteristic: The characteristic with updated value.
    ///   - error: An error object if the update failed.
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        guard let data = characteristic.value else {
            print("No data received.")
            return
        }
        
        if let jsonString = String(data: data, encoding: .utf8) {
            print("Received JSON: \(jsonString)")
            
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
            }
        }
    }
}
