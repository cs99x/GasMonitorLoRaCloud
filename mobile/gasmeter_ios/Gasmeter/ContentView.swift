import SwiftUI

/// A SwiftUI view for managing and displaying Bluetooth device data and interactions.
struct ContentView: View {
    /// The Bluetooth manager handling device discovery and connections.
    @StateObject private var bluetoothManager = BluetoothManager()

    /// The body of the view.
    var body: some View {
        NavigationView {
            VStack {
                // Display data from the connected device
                if let deviceData = bluetoothManager.deviceData {
                    VStack {
                        Text("Device: \(deviceData.id)")
                            .font(.headline)
                            .padding(.top)

                        Text("Timestamp: \(deviceData.ts)")
                        Text("Battery: \(deviceData.batt.pct)%")
                        Text("Charging: \(deviceData.batt.chg ? "Yes" : "No")")

                        // List of sensor readings
                        List(deviceData.sensors, id: \.type) { reading in
                            VStack(alignment: .leading) {
                                Text("\(reading.name) (\(reading.type))")
                                    .font(.headline)
                                Text("Value: \(reading.val) \(reading.unit)")
                            }
                        }

                        Text("Alarm Triggered: \(deviceData.stat.alarm ? "Yes" : "No")")
                        Text("Sensor Fault: \(deviceData.stat.fault ? "Yes" : "No")")
                        Text("Temperature: \(deviceData.temp.val) \(deviceData.temp.unit)")
                    }
                    .padding()
                } else {
                    // Display a list of discovered devices
                    VStack {
                        Text("Discovered Devices")
                            .font(.headline)
                            .padding()

                        if bluetoothManager.discoveredPeripherals.isEmpty {
                            Text("No devices found. Start a scan!")
                                .foregroundColor(.gray)
                                .padding()
                        } else {
                            List(bluetoothManager.discoveredPeripherals, id: \.peripheral.identifier) { item in
                                Button(action: {
                                    bluetoothManager.connectToPeripheral(item.peripheral)
                                }) {
                                    VStack(alignment: .leading) {
                                        Text(item.name)
                                            .font(.headline)
                                        Text("ID: \(item.peripheral.identifier.uuidString)")
                                            .font(.subheadline)
                                            .foregroundColor(.gray)
                                    }
                                }
                            }
                        }
                    }
                }

                Spacer()

                // Button to start scanning for devices
                Button("Scan for Devices") {
                    bluetoothManager.startScan()
                }
                .padding()
                .background(Color.blue)
                .foregroundColor(.white)
                .cornerRadius(8)
            }
            .padding()
            .navigationTitle("Bluetooth Scanner")
        }
    }
}

/// Preview provider for `ContentView`.
struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
