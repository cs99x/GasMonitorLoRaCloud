#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include <ArduinoJson.h> // ArduinoJson-Bibliothek verwenden

// Definiere den ADC-Kanal, der mit den Sensoren verbunden ist
#define MQ3_ADC_CHANNEL ADC1_CHANNEL_6   // GPIO34
#define MQ135_ADC_CHANNEL ADC1_CHANNEL_7 // GPIO35
#define MQ2_ADC_CHANNEL ADC1_CHANNEL_4   // GPIO32
#define MQ9_ADC_CHANNEL ADC1_CHANNEL_5   // GPIO33

void setup() {
    adc1_config_width(ADC_WIDTH_BIT_12); // 12-Bit-Auflösung für alle Kanäle

    // Konfiguration der Kanäle
    adc1_config_channel_atten(MQ3_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(MQ135_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(MQ2_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(MQ9_ADC_CHANNEL, ADC_ATTEN_DB_11);

    printf("Sensor-Setup abgeschlossen. JSON-Ausgabe wird gestartet...\n");
}

void loop() {
    // Sensorwerte auslesen
    int adc_reading1 = adc1_get_raw(MQ3_ADC_CHANNEL);
    int adc_reading2 = adc1_get_raw(MQ135_ADC_CHANNEL);
    int adc_reading3 = adc1_get_raw(MQ2_ADC_CHANNEL);
    int adc_reading4 = adc1_get_raw(MQ9_ADC_CHANNEL);

    // Spannungen berechnen
    float voltage1 = (adc_reading1 / 4095.0) * 3.3;
    float voltage2 = (adc_reading2 / 4095.0) * 3.3;
    float voltage3 = (adc_reading3 / 4095.0) * 3.3;
    float voltage4 = (adc_reading4 / 4095.0) * 3.3;

    // Create JSON payload
    StaticJsonDocument<512> doc;

    doc["battery_chg"] = true;
    doc["battery_pct"] = 96;
    doc["device_id"] = "SOO EIN FEUERBALL JUNGEE";
    doc["timestamp"] = "Sun, 20 Oct 2024 15:30:00 GMT";

    JsonArray sensors = doc.createNestedArray("sensors");

    JsonObject mq3 = sensors.createNestedObject();
    mq3["name"] = "MQ-3";
    mq3["type"] = "Alcohol";
    mq3["unit"] = "ppm";
    mq3["val"] = voltage1;

    JsonObject mq135 = sensors.createNestedObject();
    mq135["name"] = "MQ-135";
    mq135["type"] = "Air Quality";
    mq135["unit"] = "ppm";
    mq135["val"] = voltage2;

    JsonObject mq2 = sensors.createNestedObject();
    mq2["name"] = "MQ-2";
    mq2["type"] = "Flammable Gas";
    mq2["unit"] = "ppm";
    mq2["val"] = voltage3;

    JsonObject mq9 = sensors.createNestedObject();
    mq9["name"] = "MQ-9";
    mq9["type"] = "Carbon Monoxide";
    mq9["unit"] = "ppm";
    mq9["val"] = voltage4;

    JsonObject temperature = doc.createNestedObject("temperature");
    temperature["type"] = "Temp";
    temperature["unit"] = "°C";
    temperature["val"] = 32.23;

    JsonObject status = doc.createNestedObject("status");
    status["alarm"] = true;
    status["fault"] = true;

    // JSON-Daten in Zeichenkette umwandeln und ausgeben
    char json_string[512];
    serializeJson(doc, json_string, sizeof(json_string));
    printf("%s\n", json_string);

    vTaskDelay(1000 / portTICK_PERIOD_MS); // Warte 1 Sekunde
}

void app_main() {
    setup();
    while (true) {
        loop();
    }
}
