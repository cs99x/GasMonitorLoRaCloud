#include <SPI.h>
#include <LoRa.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
//needed for JSON LoRa communication
#include "freertos/semphr.h"

// LoRa Pins
#define LORA_SS 5
#define LORA_RST 14
#define LORA_DIO0 26
#define LORA_SCK 18
#define LORA_MISO 19
#define LORA_MOSI 23

// Analoge Pins for the Sensors
#define Sensor1_ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34 former MQ3_ADC_CHANNEL ADC1_CHANNEL_6
#define Sensor2_ADC_CHANNEL ADC1_CHANNEL_7 // GPIO35 former MQ135_ADC_CHANNEL ADC1_CHANNEL_7
#define Sensor3_ADC_CHANNEL ADC1_CHANNEL_4 // GPIO32 former MQ2_ADC_CHANNEL ADC1_CHANNEL_4
#define Sensor4_ADC_CHANNEL ADC1_CHANNEL_5 // GPIO33 former MQ9_ADC_CHANNEL ADC1_CHANNEL_5

//define Pins for the Buttons
#define BUTTON_PIN1 16
#define BUTTON_PIN2
#define BUTTON_PIN3
#define BUTTON_PIN4

// display settings
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, & Wire);


//----buzzer stuff--------
//defines the pin for the buzzer
unsigned long previousMillis = 0; // Stores the last time the buzzer state changed
const int buzzerPin = 27; // Pin for the buzzer
const int toneFrequency = 1000; // Frequency of the tone in Hz
const unsigned long toneDuration = 1000; // Duration to keep the tone on (milliseconds)
const unsigned long offDuration = 1000; // Duration to keep the tone off (milliseconds)
bool buzzerOn = false; // Tracks the current state of the buzzer
float alertValueThreshold = 2000.0; // the threshold when the buzzer goes off
//----buzzer stuff end----

//---------------defines old end---------------
//---------------numbers old-------------------

const float sensorSupplyVoltage = 5.0;

int currentButton1State = 0;
int lastButton1State = 0;
int displayCase = 0;


float alertPPMsensor1 = 1000.0; //dummy value
float alertPPMsensor2 = 40000.0; //dummy value
float alertPPMsensor3 = 100.0; //dummy value
float alertPPMsensor4 = 40.0; //dummy value

// global json object
StaticJsonDocument < 512 > doc;
char globalJson[512];

//TESTING-VALUES
int testDisplayCase = 7;
int rssiLoraDummy = 100; //dummy value
float snrLoraDummy = 10.1; //dummy value

// call own functions
void setupLoRa();
bool isButton1pressedOnce();
void barSingleGas();
float voltageToPPM();
void LoRaTask(void* parameter); // gets called by core 1
void Show4ValueBar();
void display1ValueBar(bool ,const char* ,const char* , float , float , float , int16_t , int16_t , int16_t , int16_t );
void smallReadoutAndBar();


//---------------numbers old end---------------

#define LED_PIN1 18 //for testing core1
#define LED_PIN2 19 //for testing core1

// FreeRTOS stuff
TaskHandle_t sensorTaskHandle; // dont know why, should have made comments sooner
TaskHandle_t displayTaskHandle; // dont know why, should have made comments sooner


//needed for JSON LoRa communication
char json_string[512];        // Globaler Puffer für JSON-Daten.
SemaphoreHandle_t jsonMutex;  // Mutex für die Synchronisierung.

// Sensor readout and Display and Buttons(Core 0)
void sensorDisplayTask(void * parameter) {
  while (true) {
    //-----------loop old------------
    // readout of Sensors
    int adc_reading1 = adc1_get_raw(Sensor1_ADC_CHANNEL); // 0-4094
    int adc_reading2 = adc1_get_raw(Sensor2_ADC_CHANNEL);
    int adc_reading3 = adc1_get_raw(Sensor3_ADC_CHANNEL);
    int adc_reading4 = adc1_get_raw(Sensor4_ADC_CHANNEL);

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

    // Create JSON payload
    doc["pct"] = 84; //dummy
    doc["id"] = "Handheld Device Beta"; // dummy
    doc["ts"] = 3600; // dummy

    JsonArray sensors = doc.createNestedArray("sensors");

    JsonObject sensor1json = sensors.createNestedObject();
    sensor1json["type"] = "Methan/CH4"; // MQ4
    sensor1json["val"] = ppmSensor1;

    JsonObject sensor2json = sensors.createNestedObject();
    sensor2json["type"] = "LPG"; // MQ6
    sensor2json["val"] = ppmSensor2;

    JsonObject sensor3json = sensors.createNestedObject();
    sensor3json["type"] = "CO"; // MQ7
    sensor3json["val"] = ppmSensor3;

    JsonObject sensor4json = sensors.createNestedObject();
    sensor4json["type"] = "H2"; // MQ8
    sensor4json["val"] = ppmSensor4;

    // renewing Display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);

gasmeterDisplayHandler(ppmSensor1, ppmSensor2, ppmSensor3, ppmSensor4, 
voltage1, voltage2, voltage3, voltage4, 
sensorSupplyVoltage, alertPPMsensor1, alertPPMsensor2, alertPPMsensor3, alertPPMsensor4, rssiLoraDummy, snrLoraDummy, BUTTON_PIN1);

    //handleBuzzer(ppmSensor1, alertValueThreshold);

      display.display();

  //definitly non blocking Code!
  vTaskDelay(2000/ portTICK_PERIOD_MS);
    //needed for
  if (xSemaphoreTake(jsonMutex, portMAX_DELAY)) {
    serializeJson(doc, json_string, sizeof(json_string));
    Serial.println("JSON serialized:");
    Serial.println(json_string);  // Debug JSON-Ausgabe
    xSemaphoreGive(jsonMutex);
    Serial.println("waiting2001 start");
    vTaskDelay(2001/ portTICK_PERIOD_MS);
    Serial.println("waiting2001 end");
  }
  vTaskDelay(2000/ portTICK_PERIOD_MS);
  }
  //-----------loop old end------------
}
//----------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT); // Initialize the buzzer pin

  //---------------setup old------------------

  pinMode(BUTTON_PIN1, INPUT_PULLUP); // config GPIO16 as input pin and enable the internal pull-up resistor

  // start Display
  if (!display.begin(0x3C, true)) { // 0x3C standard-I2C-Adress
    printf("Where is the Display? please tell me ( 0 _ 0 )\n");
    while (true);
  }
  display.clearDisplay();
  display.display();

  adc1_config_width(ADC_WIDTH_BIT_12); // 12 bit resolution

  adc1_config_channel_atten(Sensor1_ADC_CHANNEL, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(Sensor2_ADC_CHANNEL, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(Sensor3_ADC_CHANNEL, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(Sensor4_ADC_CHANNEL, ADC_ATTEN_DB_11);

  printf("Sensor Setup complete\n");

  //---------------setup old end--------------

  //---------------LoRa Start------------------
  // Initialize LoRa
  setupLoRa();
  
  //---------------LoRa End--------------------

  // Initialisiere den Mutex
    jsonMutex = xSemaphoreCreateMutex();
    if (jsonMutex == NULL) {
        Serial.println("Failed to create Mutex");
        while (1);
    }

  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);

  // create tasks (name,descripion,memory in bytes, optional stuff, priortity(bigger number=bigger ignore,adress of the task, used Core ))
  xTaskCreatePinnedToCore(sensorDisplayTask, "SensorDisplayTask", 8192, NULL, 1, & sensorTaskHandle, 0); // Core 0
  xTaskCreatePinnedToCore(LoRaTask, "LoRaTask", 4096, NULL, 1, & displayTaskHandle, 1); // Core 1
}

void loop() {
  //   //void loop is on vacation,FreeRTOS will be doing the work 
}

//own functions
/**
 * @brief Checks if Button 1 was pressed once.
 * @return True if Button 1 was pressed once, false if still pressed or not presseded again.
 */
bool isButtonPressedOnce(int buttonPin, int & lastButtonState) {
  if (digitalRead(buttonPin) == 0 && lastButtonState == 1) {
    vTaskDelay(20 / portTICK_PERIOD_MS); //temporary
    lastButtonState = 0;
    return true;
  } else {
    if (digitalRead(buttonPin) == 1) {
      lastButtonState = 1;
    }
    return false;
  }
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
float voltageToPPM(float liveSensorVoltage, float VCC, float Rl, float R0, float m, float b) {

  float Rs = Rl * (VCC - liveSensorVoltage) / liveSensorVoltage;
  float ratioRsR0 = Rs / R0;
  // ppm = 10^((log(Rs/R0)-b)/m) 
  float ppm = pow(10, (log10(ratioRsR0) - b) / m);
  return ppm;
}


void setupLoRa() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(915E6)) {
    Serial.println("LoRa initialization failed!");
    while (1);
  }

  Serial.println("LoRa initialized.");
}

void LoRaTask(void* parameter) {
    while (true) {
        Serial.println("Start: " + xTaskGetTickCount() / 1000);
        Serial.println("LoRaTask running...");
        if (xSemaphoreTake(jsonMutex, portMAX_DELAY)) {
            char localBuffer[512];
            strcpy(localBuffer, json_string);
            xSemaphoreGive(jsonMutex);

            Serial.println("Preparing to send via LoRa...");
            Serial.println(localBuffer);
            Serial.println("LoRa Test out 1");
            

            LoRa.beginPacket();
            Serial.println("LoRa Test out 2");
            LoRa.print(localBuffer);
            Serial.println("LoRa Test out 3");
            Serial.println("LoRa Test out 4");
            if (LoRa.endPacket(true)) {
              Serial.println("LoRa packet sent successfully.");
            } else {
              Serial.println("LoRa packet failed to send!");
            }

            Serial.println("LoRa packet sent successfully.");
        }
        Serial.println("Stop: " + xTaskGetTickCount() / 1000);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void Show4ValueBar(float ppmSensor1, float ppmSensor2, float ppmSensor3, float ppmSensor4, 
                       float voltage1, float voltage2, float voltage3, float voltage4) {
    display.clearDisplay();
    display.invertDisplay(false);

    display.setCursor(0, 0);
    display.printf("MQ-3: %.2f ppm\n", ppmSensor1);
    barSingleGas(voltage1, 5.0, 6, 110, 0, 7);

    display.setCursor(0, 14);
    display.printf("MQ-135: %.2f ppm\n", ppmSensor2);
    barSingleGas(voltage2, 5.0, 6, 110, 0, 21);

    display.setCursor(0, 28);
    display.printf("MQ-2: %.2f ppm\n", ppmSensor3);
    barSingleGas(voltage3, 5.0, 6, 110, 0, 35);

    display.setCursor(0, 42);
    display.printf("MQ-9: %.2f ppm\n", ppmSensor4);
    barSingleGas(voltage4, 5.0, 6, 110, 0, 49);

    display.display();
}

void display1ValueBar(bool inversion,const char* sensorName,const char* gas, float ppmSensor, float voltageSensor, float sensorSupplyVoltage, int16_t barHeight, int16_t barWidth, int16_t barStartX, int16_t barStartY){
  display.clearDisplay();
      display.invertDisplay(inversion);
      display.setCursor(0, 0);
      display.printf("%s: %.2f ppm\n", sensorName, ppmSensor);
      display.printf("%s", gas);
      barSingleGas(voltageSensor, sensorSupplyVoltage, barHeight, barWidth, barStartX, barStartY);
      display.display();
}

void smallReadoutAndBar(bool invertDisplay,int16_t textStartX,int16_t textStartY, const char* sensorName, float  ppmSensor, float voltageRadout, float maxSensorVoltage){
  display.setTextSize(1);
  display.clearDisplay();
  display.invertDisplay(invertDisplay);
  display.setCursor(textStartX, textStartY);
  display.printf("%s : %.2f ppm\n",sensorName, ppmSensor);
  barSingleGas(voltageRadout,maxSensorVoltage , 6, 110, textStartX,textStartY + 7 );

}

void gasmeterDisplayHandler(float ppmSensor1, 
  float ppmSensor2, 
  float ppmSensor3, 
  float ppmSensor4, 
  float voltage1, 
  float voltage2, 
  float voltage3, 
  float voltage4, 
  float sensorSupplyVoltage, 
  float alertPPMsensor1, 
  float alertPPMsensor2, 
  float alertPPMsensor3, 
  float alertPPMsensor4, 
  int rssiLora, 
  float snrLora, 
  int buttonPin) {
    static int currentButtonState = 0;
    static int lastButtonState = 0; // Persistent variable to track the last button state
    static int persistentDisplayCase = 0; // Persistent display case variable

    switch (persistentDisplayCase) {
        case 0:
            display.clearDisplay();
            smallReadoutAndBar(false, 0, 0, "Sensor1", ppmSensor1, voltage1, 5.0);
            smallReadoutAndBar(false, 0, 14, "Sensor2", ppmSensor2, voltage2, 5.0);
            smallReadoutAndBar(false, 0, 28, "Sensor3", ppmSensor3, voltage3, 5.0);
            smallReadoutAndBar(false, 0, 42, "Sensor4", ppmSensor4, voltage4, 5.0);
            display.display();
            break;

        case 1:
            display1ValueBar(false, "Sensor1Test", "_\-", ppmSensor1, voltage1, sensorSupplyVoltage, 20, 120, 4, 30);
            break;

        case 2:
            display1ValueBar(false, "Sensor2Test", "\-\_", ppmSensor2, voltage2, sensorSupplyVoltage, 20, 120, 4, 30);
            break;

        case 3:
            display1ValueBar(false, "Sensor3Test", "\.\-", ppmSensor3, voltage3, sensorSupplyVoltage, 20, 120, 4, 30);
            break;

        case 4:
            display1ValueBar(false, "Sensor4Test", "\-\.", ppmSensor4, voltage4, sensorSupplyVoltage, 20, 120, 4, 30);
            break;

        case 5:
            display.clearDisplay();
            display.invertDisplay(false);
            display.setCursor(0, 0);
            display.printf("Battery: --%%\n");
            display.printf("      ALERT at:\n");
            display.printf("Gas 1: %.2fppm\n", alertPPMsensor1);
            display.printf("Gas 2: %.2fppm\n", alertPPMsensor2);
            display.printf("Gas 3: %.2fppm\n", alertPPMsensor3);
            display.printf("Gas 4: %.2fppm\n", alertPPMsensor4);
            display.display();
            break;

        case 6:
            display.clearDisplay();
            display.invertDisplay(false);
            display.setCursor(0, 0);
            display.setTextSize(1);
            display.printf("LoRa Connection Data\n");
            display.setTextSize(2);
            display.printf("RSSI: %i\n", rssiLora);
            display.printf("SNR:  %.1f \n", snrLora);
            display.display();
            break;

        default:
            display.clearDisplay();
            display.invertDisplay(false);
            display.setCursor(0, 0);
            display.printf("Buttonstate: %i \n", currentButton1State);
            display.printf("Case: %i \n", persistentDisplayCase);
            display.printf("reset needed!");
            display.display();
            break;
    }

    // Check for button press and update the persistent display case
    if (isButtonPressedOnce(buttonPin, lastButtonState)) {
        persistentDisplayCase++;
        if (persistentDisplayCase >= 7) {
            persistentDisplayCase = 0;
        }
    }

    lastButtonState = currentButtonState; // Update the last button state
}
/*
void handleBuzzer(float alertTest, float alertValueThreshold) {
  if (alertTest >= alertValueThreshold) {
    unsigned long currentMillis = millis();
    if (buzzerOn) {
      // Turn off the tone after toneDuration
      if (currentMillis - previousMillis >= toneDuration) {
        noTone(buzzerPin);
        buzzerOn = false;
        previousMillis = currentMillis; // Reset the timer
      }
    } else {
      // Turn on the tone after offDuration
      if (currentMillis - previousMillis >= offDuration) {
        tone(buzzerPin, toneFrequency);
        buzzerOn = true;
        previousMillis = currentMillis; // Reset the timer
      }
    }
  } else {
    // Ensure the buzzer is off if alertTest > 2000.0
    noTone(buzzerPin);
    buzzerOn = false;
  }
}
void handleBuzzer(float alertTest, float alertValueThreshold) {
  if (alertTest >= alertValueThreshold) {
    unsigned long currentMillis = millis();
    if (buzzerOn) {
      // Turn off the tone after toneDuration
      if (currentMillis - previousMillis >= toneDuration) {
        noTone(buzzerPin);
        buzzerOn = false;
        previousMillis = currentMillis; // Reset the timer
      }
    } else {
      // Turn on the tone after offDuration
      if (currentMillis - previousMillis >= offDuration) {
        tone(buzzerPin, toneFrequency);
        buzzerOn = true;
        previousMillis = currentMillis; // Reset the timer
      }
    }
  } else {
    // Ensure the buzzer is off if alertTest > 2000.0
    noTone(buzzerPin);
    buzzerOn = false;
  }
}
*/


