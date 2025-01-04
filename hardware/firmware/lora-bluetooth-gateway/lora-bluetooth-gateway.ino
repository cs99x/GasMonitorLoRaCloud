#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

#define SERVICE_UUID        "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-abc123456789"

// LoRa Pins
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

// LoRa Reception Variables
String receivedPayload = "";
SemaphoreHandle_t dataMutex;

// FreeRTOS Task Handles
TaskHandle_t bleTaskHandle = NULL;
TaskHandle_t loraTaskHandle = NULL;

// BLE Server Callbacks
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        deviceConnected = true;
        Serial.println("Device connected! Starting LoRa reception...");
        vTaskResume(loraTaskHandle); // Resume LoRa task
    }

    void onDisconnect(BLEServer* pServer) override {
        deviceConnected = false;
        Serial.println("Device disconnected! Stopping LoRa reception...");
        vTaskSuspend(loraTaskHandle); // Suspend LoRa task
    }
};

// Task: Process and Notify BLE
void bleTask(void *pvParameters) {
    while (true) {
        if (deviceConnected) {
            if (xSemaphoreTake(dataMutex, portMAX_DELAY)) {
                if (!receivedPayload.isEmpty()) {
                    // Send the JSON payload via BLE
                    pCharacteristic->setValue(receivedPayload.c_str());
                    pCharacteristic->notify();
                    Serial.println("JSON payload sent via BLE:");
                    Serial.println(receivedPayload);

                    // Clear payload
                    receivedPayload = "";
                }
                xSemaphoreGive(dataMutex);
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS); // Check every 500ms
    }
}

// Task: LoRa Reception
void loraTask(void *pvParameters) {
    while (true) {
        int packetSize = LoRa.parsePacket();
        if (packetSize) {
            String payload = "";
            char receivedChar;
            Serial.println("LoRa packet received.");

            while (LoRa.available()) {
                receivedChar = LoRa.read();
                payload += receivedChar;
            }

            if (xSemaphoreTake(dataMutex, portMAX_DELAY)) {
                receivedPayload = payload;
                Serial.println("Complete JSON payload received:");
                Serial.println(receivedPayload);
                xSemaphoreGive(dataMutex);
            }

            Serial.println(); // Newline for clarity
        }
        vTaskDelay(100 / portTICK_PERIOD_MS); // Poll LoRa every 100ms
    }
}

// Initialize LoRa
void setupLoRa() {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(915E6)) {
        Serial.println("LoRa initialization failed!");
        while (1);
    }

    Serial.println("LoRa initialized.");
}

void setup() {
    Serial.begin(115200);

    // Initialize BLE
    BLEDevice::init("ESP32_LoRa_BLE");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();
    BLEDevice::startAdvertising();
    Serial.println("BLE advertising started.");

    // Initialize LoRa
    setupLoRa();

    // Create Mutex for Data Synchronization
    dataMutex = xSemaphoreCreateMutex();

    // Create FreeRTOS Tasks
    xTaskCreatePinnedToCore(bleTask, "BLE Task", 4096, NULL, 1, &bleTaskHandle, 1);
    xTaskCreatePinnedToCore(loraTask, "LoRa Task", 4096, NULL, 1, &loraTaskHandle, 1);

    // Start with LoRa task suspended
    vTaskSuspend(loraTaskHandle);
}

void loop() {
    // FreeRTOS handles everything
}
