#include <SPI.h>
#include <LoRa.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>

// LoRa Pins
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23

#define MAX_LORA_PAYLOAD 255 // Maximum payload size for LoRa

// OLED Display
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Task Handles
TaskHandle_t loRaTaskHandle = NULL;
TaskHandle_t oledDisplayTaskHandle = NULL;

// Global JSON Data
StaticJsonDocument<512> globalDoc;
SemaphoreHandle_t jsonMutex;

// Function to initialize LoRa
void setupLoRa() {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(915E6)) {
        Serial.println("LoRa initialization failed!");
        while (1);
    }

    Serial.println("LoRa initialized.");
}

// Function to send JSON payload in chunks
void sendLoRaChunked(const char *jsonPayload) {
    size_t length = strlen(jsonPayload);
    size_t offset = 0;

    while (offset < length) {
        size_t chunkSize = std::min((size_t)(length - offset), (size_t)MAX_LORA_PAYLOAD);

        LoRa.beginPacket();
        LoRa.write((uint8_t *)(jsonPayload + offset), chunkSize);
        LoRa.endPacket();

        Serial.printf("Sent chunk: %.*s\n", chunkSize, jsonPayload + offset);

        offset += chunkSize;

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// FreeRTOS task for LoRa transmission
void loRaTask(void *parameter) {
    while (true) {
        // Create JSON payload
        StaticJsonDocument<512> doc;

        doc["pct"] = 84;
        doc["id"] = "99% von euch sind RATTEN!";
        doc["ts"] = 3600;

        JsonArray sensors = doc.createNestedArray("sensors");

        JsonObject methane = sensors.createNestedObject();
        methane["type"] = "CH4";
        methane["val"] = 129.35;

        JsonObject co2 = sensors.createNestedObject();
        co2["type"] = "CO2";
        co2["val"] = 404.43;

        JsonObject oxygen = sensors.createNestedObject();
        oxygen["type"] = "O2";
        oxygen["val"] = 21.83;

        JsonObject co = sensors.createNestedObject();
        co["type"] = "CO";
        co["val"] = 35.97;

        char jsonString[512];
        serializeJson(doc, jsonString, sizeof(jsonString));

        // Lock the mutex to update the global JSON
        if (xSemaphoreTake(jsonMutex, portMAX_DELAY)) {
            globalDoc = doc;
            xSemaphoreGive(jsonMutex);
        }

        Serial.println("Full JSON Payload:");
        Serial.println(jsonString);

        // Send JSON payload in chunks
        sendLoRaChunked(jsonString);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

// FreeRTOS task for OLED display
void oledDisplayTask(void *parameter) {
    while (true) {
        // Lock the mutex to read the global JSON
        if (xSemaphoreTake(jsonMutex, portMAX_DELAY)) {
            if (!globalDoc.isNull()) {
                u8g2.clearBuffer();

                // Display ID and battery info
                u8g2.setFont(u8g2_font_ncenB08_tr);
                u8g2.setCursor(0, 10);
                u8g2.print("ID: ");
                u8g2.print(globalDoc["id"].as<const char *>());

                u8g2.setCursor(0, 25);
                u8g2.print("Battery: ");
                u8g2.print(globalDoc["pct"].as<int>());
                u8g2.print("%");

                // Display sensor data
                JsonArray sensors = globalDoc["sensors"];
                int y = 40;
                for (JsonObject sensor : sensors) {
                    u8g2.setCursor(0, y);
                    u8g2.print(sensor["type"].as<const char *>());
                    u8g2.print(": ");
                    u8g2.print(sensor["val"].as<float>());
                    y += 15; // Move to the next line
                }

                // Update the display
                u8g2.sendBuffer();
            }
            xSemaphoreGive(jsonMutex);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);

    // Initialize LoRa
    setupLoRa();

    // Initialize OLED
    u8g2.begin();

    // Display initialization message
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 10);
    u8g2.print("Initializing...");
    u8g2.sendBuffer();

    // Create mutex for JSON synchronization
    jsonMutex = xSemaphoreCreateMutex();

    // Create FreeRTOS tasks
    xTaskCreatePinnedToCore(loRaTask, "LoRaTask", 4096, NULL, 1, &loRaTaskHandle, 1);
    xTaskCreatePinnedToCore(oledDisplayTask, "OLEDTask", 4096, NULL, 1, &oledDisplayTaskHandle, 1);
}

void loop() {
    // Nothing to do in the main loop, FreeRTOS handles everything
}
