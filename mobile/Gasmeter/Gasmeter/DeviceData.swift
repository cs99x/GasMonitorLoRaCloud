struct DeviceData: Codable {
    let id: String
    let ts: String
    let batt: BatteryStatus
    let sensors: [SensorReading]
    let temp: Temperature
    let stat: Status

    struct BatteryStatus: Codable {
        let pct: Int
        let chg: Bool
    }

    struct SensorReading: Codable {
        let type: String
        let name: String
        let unit: String
        let val: Double
    }

    struct Temperature: Codable {
        let type: String
        let unit: String
        let val: Double
    }

    struct Status: Codable {
        let alarm: Bool
        let fault: Bool
    }
}
