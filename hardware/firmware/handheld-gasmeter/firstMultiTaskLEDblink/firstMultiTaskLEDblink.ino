#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// OLED settings
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire);

#define LED_PIN1 18
#define LED_PIN2 19

// FreeRTOS Variablen
TaskHandle_t sensorTaskHandle;
TaskHandle_t displayTaskHandle;
SemaphoreHandle_t jsonMutex;


// Sensor readout and Display maybe some day (Core 0)
void sensorDisplayTask(void *parameter) {
    while (true) {

    digitalWrite(LED_PIN1, HIGH);
    delay(500);
    digitalWrite(LED_PIN1, LOW);
    delay(500);
    }
}

// LoRa maybe in the close future(Kern 1)
void LoRaTask(void *parameter) {
    while (true) {
    digitalWrite(LED_PIN2, HIGH);
    delay(250);
    digitalWrite(LED_PIN2, LOW);
    delay(250);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(LED_PIN1, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);

    // create tasks (name,descripion,memory in bytes, optional stuff, priortity(bigger number=bigger ignore,adress of the task, used Core ))
    xTaskCreatePinnedToCore(sensorDisplayTask, "SensorDisplayTask", 8192, NULL, 1, &sensorTaskHandle, 0); // Core 0
    xTaskCreatePinnedToCore(LoRaTask, "LoRaTask", 4096, NULL, 1, &displayTaskHandle, 1); // Core 1
}

void loop() {
    //   //void loop is on vacation,FreeRTOS will be doing the work 
}
