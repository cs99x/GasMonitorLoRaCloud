#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <SPI.h>
#include <LoRa.h>


// Pin-Definitionen
// Pins for LoRa
#define LORA_MISO 12
#define LORA_MOSI 11
#define LORA_SCK 14
#define NSS_PIN 10
#define LORA_RST 5
#define LORA_DIO0 6
//Pins for Sensors
#define Sensor1_ADC_CHANNEL 1   
#define Sensor2_ADC_CHANNEL 2   
#define Sensor3_ADC_CHANNEL 3   
#define Sensor4_ADC_CHANNEL 4  

#define sensorSupplyVoltage 5.0 


// America Frequency(915 MHz)
#define LORA_FREQUENCY 915E6

float voltageToPPM();
float sendJsonMessage();

void setup() {
  Serial.begin(9600);
  while (!Serial);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, NSS_PIN);
  LoRa.setPins(NSS_PIN, LORA_RST, LORA_DIO0);

  // Initialize LoRa module
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa duplex communication started.");
}

void loop() {


  // analog readout of Sensor Pins
  int adc_reading1 =  analogRead(Sensor1_ADC_CHANNEL);
  int adc_reading2 =  analogRead(2);
  int adc_reading3 =  analogRead(3);
  int adc_reading4 =  analogRead(4);

  // calculating Voltage of the Sensor
  float voltage1 = (adc_reading1 / 4095.0) * sensorSupplyVoltage;
  float voltage2 = (adc_reading2 / 4095.0) * sensorSupplyVoltage;
  float voltage3 = (adc_reading3 / 4095.0) * sensorSupplyVoltage;
  float voltage4 = (adc_reading4 / 4095.0) * sensorSupplyVoltage;

  //calculating ppm
  //for MQ9 sensor for CO
  float ppmSensor4 = voltageToPPM(voltage4, sensorSupplyVoltage, 10000, 5000, -0.77, 1.0);
  // for MQ2 sensor for Lighter gas
  float ppmSensor3 = voltageToPPM(voltage3, sensorSupplyVoltage, 10000, 5000, -1.394, 3.0);
  // for MQ135 Sensor for CO2
  float ppmSensor2 = voltageToPPM(voltage2, sensorSupplyVoltage, 10000, 5000, -0.45, 2.85);
  // for MQ3 Sensor for Alkohol (Ethanol)
  float ppmSensor1 = voltageToPPM(voltage1, sensorSupplyVoltage, 10000, 5000, -0.62, 2.3);

    // Sende JSON-Nachricht
  sendJsonMessage(ppmSensor1,ppmSensor2,ppmSensor3,ppmSensor4);

  // Warte eine Sekunde, bevor die Werte erneut ausgelesen werden
  delay(5000);
}

void sendJsonMessage(float ppmSensor1, float ppmSensor2, float ppmSensor3, float ppmSensor4) {
  StaticJsonDocument<255> doc;

  doc["battery_pct"] = 84;
  doc["device_id"] = "NewBoard1";
  doc["timestamp"] = "Sun, 20 Oct 2024 15:30:00 GMT";

  JsonArray sensors = doc.createNestedArray("sensors");

  JsonObject methane = sensors.createNestedObject();
  methane["type"] = "CH4";
  methane["val"] = ppmSensor1;

  JsonObject co2 = sensors.createNestedObject();
  co2["type"] = "CO2";
  co2["val"] = ppmSensor2;

  JsonObject lighterGas = sensors.createNestedObject();
  lighterGas["type"] = "LPG";
  lighterGas["val"] = ppmSensor3;

  JsonObject co = sensors.createNestedObject();
  co["type"] = "CO";
  co["val"] = ppmSensor4;


  char jsonString[255];
  serializeJson(doc, jsonString, sizeof(jsonString));

  Serial.print("Sending JSON: ");
  Serial.println(jsonString);

  LoRa.beginPacket();
  LoRa.write((uint8_t*)jsonString,sizeof(jsonString));
  LoRa.endPacket();
}

float voltageToPPM(float liveSensorVoltage, float VCC, float Rl, float R0, float m, float b){


  float Rs = Rl * (VCC - liveSensorVoltage) / liveSensorVoltage;
  
  float ratioRsR0 = Rs / R0;

  // ppm = 10^((log(Rs/R0)-b)/m) 
  float ppm = pow(10, (log10(ratioRsR0) - b) /m);

  return ppm;
}