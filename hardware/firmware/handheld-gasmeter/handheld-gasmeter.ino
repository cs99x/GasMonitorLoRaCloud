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

// needed to use display
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire);

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

// american LoRa Frequency
#define LORA_FREQUENCY 915E6

// own definitions
float sensorSupplyVoltage = 5.0;

int16_t defineBarHeight = 16;
int16_t defineBarWidth = 110;
int16_t defineBarStartX = 10;
int16_t defineBarStartY = 30;

float voltageToPPM();
float sendJsonMessage();
void displayDriver(float, float, float, float, float, float, float, float, float);
void drawSmokeAnimation();

void setup() {
  Serial.begin(115200);
  while (!Serial);

  //display
  Wire.begin(8, 9);
  Serial.println("I2C startet");

  if (!display.begin(0x3C)) { // Standard-I2C-Adresse für SH1106
    Serial.println("Display not working, fix that!");
    while (1);
  }
  display.clearDisplay();
  display.display();
  Serial.println("Display started");

  //Lora 
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, NSS_PIN);
  LoRa.setPins(NSS_PIN, LORA_RST, LORA_DIO0);

  // Initialize LoRa module
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed, fix that!");
    while (1);
  }
  Serial.println("LoRa communication started.");

  // pin for button 1
  pinMode(35, INPUT_PULLDOWN);
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
  Serial.println("Start sending JSON with LoRa");
  sendJsonMessage(ppmSensor1,ppmSensor2,ppmSensor3,ppmSensor4);
  delay(20);
  Serial.println("End JSON sending");
  delay(20);
  displayDriver(ppmSensor1, ppmSensor2, ppmSensor3, ppmSensor4, voltage1, voltage2, voltage3, voltage4, 5.0);
}

/**
 * @brief Sends the ppm from gases with LoRa (with placeholder for id, battery and a timestamp).
 *
 * @param ppmSensor1 ppm of the first sensor.
 * @param ppmSensor2 ppm of the secound sensor.
 * @param ppmSensor3 ppm of the third sensor.
 * @param ppmSensor4 ppm of the fourth sensor.
 */
void sendJsonMessage(float ppmSensor1, float ppmSensor2, float ppmSensor3, float ppmSensor4) {
  StaticJsonDocument<255> doc;

  doc["pct"] = 84; //Batery percentage
  doc["id"] = "1"; //Identification
  doc["ts"] = 3600; // timestamp

  JsonArray sensors = doc.createNestedArray("sensors");

  JsonObject methane = sensors.createNestedObject();
  methane["type"] = "CH4";
  methane["val"] = ppmSensor1;

  JsonObject co2 = sensors.createNestedObject();
  co2["type"] = "CO";
  co2["val"] = ppmSensor2;

  JsonObject lighterGas = sensors.createNestedObject();
  lighterGas["type"] = "LPG";
  lighterGas["val"] = ppmSensor3;

  JsonObject co = sensors.createNestedObject();
  co["type"] = "H2";
  co["val"] = ppmSensor4;


  char jsonString[255];
  serializeJson(doc, jsonString, sizeof(jsonString));

  Serial.print("Sending JSON: ");
  Serial.println(jsonString);

  LoRa.beginPacket();
  LoRa.write((uint8_t*)jsonString,sizeof(jsonString));
  LoRa.endPacket();
  delay(20);
}

/**
 * @brief Converts the live sensor voltage to a gas concentration in parts per million to be used in with MQ series sensors (ppm).
 *
 * This function uses the sensor's live voltage, supply voltage, load resistance, calibration resistance,
 * and logarithmic curve parameters to calculate the gas concentration in ppm.
 *
 * @param liveSensorVoltage The live output voltage of the sensor (in volts).
 * @param VCC The supply voltage provided to the sensor (in volts).
 * @param Rl The load resistance of the sensor (internal)(in ohms).
 * @param R0 The calibration resistance of the sensor (can be adjusted on the SensorHardware) (in ohms).
 * @param m Logarithmic curve specific for the sensor (in Datashee somtimes "slope or alpha") otherwise calculate with datasheet
 * @param b A constant indicating the gas concentration when Rs/R0 is 1 for a specific gas.
 *          Can be calculated as b = log10(ppm for a known Rs/R0) - m * log10(Rs/R0).
 *
 * @return The gas concentration in ppm as a float.
 */
float voltageToPPM(float liveSensorVoltage, float VCC, float Rl, float R0, float m, float b){


  float Rs = Rl * (VCC - liveSensorVoltage) / liveSensorVoltage;
  
  float ratioRsR0 = Rs / R0;

  // ppm = 10^((log(Rs/R0)-b)/m) 
  float ppm = pow(10, (log10(ratioRsR0) - b) /m);

  return ppm;
}

/**
 * @brief shows the diferrent sensordata in combination with a button.
 *
 * @param ppmSensor1 ppm of the first sensor.
 * @param ppmSensor2 ppm of the secound sensor.
 * @param ppmSensor3 ppm of the third sensor.
 * @param ppmSensor4 ppm of the fourth sensor.
 * @param voltage1 the voltage of the analog output of the first sensor.
 * @param voltage2 the voltage of the analog output of the secound sensor.
 * @param voltage4 the voltage of the analog output of the third sensor.
 * @param voltage4 the voltage of the analog output of the fourth sensor.
 * @param sensorSupplyVoltage the Voltage supplied to the sensors.
 */
void displayDriver(float ppmSensor1, 
  float ppmSensor2, 
  float ppmSensor3, 
  float ppmSensor4, 
  float voltage1, 
  float voltage2, 
  float voltage3, 
  float voltage4, 
  float sensorSupplyVoltage){
  
  delay(20);
  
  static int DisplayCase = 1;
  static int currentButtonState = 0;
  static int lastButtonState = 0; 

  switch (DisplayCase) {
    case 0:
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(0, 0);
      display.printf("CH4: %.1f ppm\n", ppmSensor1);
      display.printf("CO : %.1f ppm\n", ppmSensor2);
      display.printf("LPG: %.1f ppm\n", ppmSensor3);
      display.printf("H4 : %.1f ppm\n", ppmSensor4);
      display.display();
        break;

    case 1:
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(0, 0);
      display.printf("Sensor1: %.1f ppm\n", ppmSensor1);
      barSingleGas(voltage1, sensorSupplyVoltage, defineBarHeight, defineBarWidth, defineBarStartX, defineBarStartY );
      display.display();
      break;

    case 2:
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(0, 0);
      display.printf("Sensor2: %.1f ppm\n", ppmSensor2);
      barSingleGas(voltage2, sensorSupplyVoltage, defineBarHeight, defineBarWidth, defineBarStartX, defineBarStartY );
      display.display();
      break;

    case 3:
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(0, 0);
      display.printf("Sensor3: %.1f ppm\n", ppmSensor3);
      barSingleGas(voltage3, sensorSupplyVoltage, defineBarHeight, defineBarWidth, defineBarStartX, defineBarStartY );
      display.display();
      break;

    case 4:
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SH110X_WHITE);
      display.setCursor(0, 0);
      display.printf("Sensor4: %.1f ppm\n", ppmSensor4);
      barSingleGas(voltage4, sensorSupplyVoltage, defineBarHeight, defineBarWidth, defineBarStartX, defineBarStartY );
      display.display();
      break;

    default:
      display.clearDisplay();
      display.invertDisplay(false);
      display.setCursor(0, 0);
      display.printf("Case: %i \n", DisplayCase);
      display.printf("reset needed!");
      display.display();
      break;
  }


  Serial.println("ask button");
  if (isButtonPressedOnce(35)) {
    DisplayCase++;
    if (DisplayCase >= 5) {
          DisplayCase = 0;
    }
  }
  delay(20);
}


/**
 * @brief Draws bar from left to right on the display based on the live sensor voltage.
 *
 * @param liveSensorVoltage The live output voltage of the sensor (in volts).
 * @param maxVoltage The maximum possible voltage output of the sensor (in volts).
 * @param barHeight The height of the bar (in pixels).
 * @param barWidth The total width of the bar (in pixels).
 * @param barStartX The X-coordinate of the bar's starting position on the display.
 * @param barStartY The Y-coordinate of the bar's starting position on the display.
 */
void barSingleGas(float liveSensorVoltage, float maxVoltage, int16_t barHeight, int16_t barWidth, int16_t barStartX, int16_t barStartY) {

  //how much of the bar needs to be filled out
  int filledWidth = (liveSensorVoltage / maxVoltage) * barWidth;

  // borderRecktangel
  display.drawRect(barStartX, barStartY, barWidth, barHeight, SH110X_WHITE);

  // fill out bar 2
  display.fillRect(barStartX, barStartY, filledWidth, barHeight, SH110X_WHITE);
}
/**
 * @brief Checks if Button 1 was pressed once.
 * @return True if Button 1 was pressed once, false if still pressed or not presseded again.
 */
bool isButtonPressedOnce(int buttonPin) {
  static int lastButtonState = 0;
  if (digitalRead(buttonPin) == HIGH && lastButtonState == 0) {
    vTaskDelay(20 / portTICK_PERIOD_MS); //temporary
    lastButtonState = 1;
    return true;
  } else {
    if (digitalRead(buttonPin) == LOW) {
      lastButtonState = 0;
    }
    return false;
  }
}