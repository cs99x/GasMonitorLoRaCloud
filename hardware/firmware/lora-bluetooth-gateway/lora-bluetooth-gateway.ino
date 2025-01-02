#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <ArduinoJson.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-abc123456789"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool isAdvertising = false; // Flag to track advertising state

// FreeRTOS task handle
TaskHandle_t jsonTaskHandle = NULL;

// Callbacks for connection events
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("Device connected!");
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("Device disconnected!");
        isAdvertising = false; // Mark advertising as stopped
        vTaskDelete(jsonTaskHandle); // Delete the task when the device disconnects
        jsonTaskHandle = NULL;
    }
};

void setup() {
    Serial.begin(115200);

    // Initialize BLE
    BLEDevice::init("ESP32_Gasmeter");
    BLEDevice::setMTU(512); // Increase MTU to 512 bytes
    BLEServer *pServer = BLEDevice::createServer();

    // Set callbacks for connection events
    pServer->setCallbacks(new MyServerCallbacks());

    // Create BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

    // Start the BLE Service
    pService->start();

    // Start advertising
    startAdvertising();

    Serial.println("BLE advertising started...");
}

// Task for generating JSON and sending BLE notifications
void jsonTask(void *parameter) {
    const unsigned long interval = 1000; // 1-second interval
    unsigned long previousMillis = millis();

    while (true) {
        if (deviceConnected) {
            unsigned long currentMillis = millis();
            if (currentMillis - previousMillis >= interval) {
                previousMillis = currentMillis;

                // Generate JSON
                String jsonString = generateJson();
                Serial.println("Generated JSON: " + jsonString);

                // Send JSON
                pCharacteristic->setValue(jsonString.c_str());
                pCharacteristic->notify();
            }
        } else {
            vTaskDelay(100 / portTICK_PERIOD_MS); // Sleep task if device not connected
        }
    }
}

void loop() {
    if (deviceConnected && jsonTaskHandle == NULL) {
        // Create the JSON task when a device connects
        xTaskCreatePinnedToCore(
            jsonTask,          // Task function
            "JSONTask",        // Task name
            4096,              // Stack size in bytes
            NULL,              // Task input parameter
            1,                 // Priority
            &jsonTaskHandle,   // Task handle
            1                  // Core to run the task on
        );
    }
}

// Generate random JSON data
String generateJson() {
    StaticJsonDocument<512> doc;

    doc["id"] = "Gasmeter v1";
    doc["ts"] = "2024-10-20T15:30:00Z"; // Placeholder, could use real-time logic

    JsonObject batt = doc.createNestedObject("batt");
    batt["pct"] = random(0, 101); // Random battery percentage
    batt["chg"] = true;   // Random boolean: 0 or 1

    JsonArray sensors = doc.createNestedArray("sensors");

    JsonObject ch4 = sensors.createNestedObject();
    ch4["type"] = "CH4";
    ch4["name"] = "Methane";
    ch4["unit"] = "ppm";
    ch4["val"] = random(50, 200) + random(0, 100) / 100.0;

    JsonObject co2 = sensors.createNestedObject();
    co2["type"] = "CO2";
    co2["name"] = "CO2";
    co2["unit"] = "ppm";
    co2["val"] = random(400, 600) + random(0, 100) / 100.0;

    JsonObject o2 = sensors.createNestedObject();
    o2["type"] = "O2";
    o2["name"] = "Oxygen";
    o2["unit"] = "%";
    o2["val"] = random(19, 22) + random(0, 100) / 100.0;

    JsonObject co = sensors.createNestedObject();
    co["type"] = "CO";
    co["name"] = "CO";
    co["unit"] = "ppm";
    co["val"] = random(20, 50) + random(0, 100) / 100.0;

    JsonObject temp = doc.createNestedObject("temp");
    temp["type"] = "Temp";
    temp["unit"] = "°C";
    temp["val"] = random(15, 35) + random(0, 100) / 100.0;

    JsonObject stat = doc.createNestedObject("stat");
    stat["alarm"] = true;
    stat["fault"] = true;

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}

// Start BLE advertising
void startAdvertising() {
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->start();
    isAdvertising = true;
    Serial.println("Restarted BLE advertising...");
}
