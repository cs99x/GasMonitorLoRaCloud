#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include <ArduinoJson.h> // ArduinoJson-Bibliothek verwenden
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/*
#define MQ3_ADC_CHANNEL ADC1_CHANNEL_6   // GPIO34
#define MQ135_ADC_CHANNEL ADC1_CHANNEL_7 // GPIO35
#define MQ2_ADC_CHANNEL ADC1_CHANNEL_4   // GPIO32
#define MQ9_ADC_CHANNEL ADC1_CHANNEL_5   // GPIO33
*/

#define Sensor1_ADC_CHANNEL ADC1_CHANNEL_6   // GPIO34 former MQ3_ADC_CHANNEL ADC1_CHANNEL_6
#define Sensor2_ADC_CHANNEL ADC1_CHANNEL_7   // GPIO35 former MQ135_ADC_CHANNEL ADC1_CHANNEL_7
#define Sensor3_ADC_CHANNEL ADC1_CHANNEL_4   // GPIO32 former MQ2_ADC_CHANNEL ADC1_CHANNEL_4
#define Sensor4_ADC_CHANNEL ADC1_CHANNEL_5   // GPIO33 former MQ9_ADC_CHANNEL ADC1_CHANNEL_5

// Definiere SH1106 Display-Einstellungen
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire);

//define GPIO19 pin conected to button1
#define BUTTON_PIN1 16

//const float maxInternalSensorVoltage = 3.3;
const float sensorSupplyVoltage = 5.0;

int currentButten1State = 0;
int lastButten1State = 0;
int displayCase = 0;

float alertPPMsensor1 = 1000.0;
float alertPPMsensor2 = 40000.0;
float alertPPMsensor3 = 100.0;
float alertPPMsensor4 = 40.0;


//------TESTING-VALUES----------
int testDisplayCase = 7;
int rssiLoraDummy = 100;
float snrLoraDummy =10.1;

void setup() {

  pinMode(16, INPUT_PULLUP); // config GPIO16 as input pin and enable the internal pull-up resistor

    // Display initialisieren
    if (!display.begin(0x3C, true)) { // 0x3C ist die Standard-I2C-Adresse
        printf("Display nicht gefunden. Überprüfe die Verkabelung!\n");
        while (true);
    }
    display.clearDisplay();
    display.display();

    adc1_config_width(ADC_WIDTH_BIT_12); // 12-Bit-Auflösung für alle Kanäle

    /* old
    adc1_config_channel_atten(MQ3_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(MQ135_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(MQ2_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(MQ9_ADC_CHANNEL, ADC_ATTEN_DB_11);
    */
    // channel configuration
    adc1_config_channel_atten(Sensor1_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(Sensor2_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(Sensor3_ADC_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(Sensor4_ADC_CHANNEL, ADC_ATTEN_DB_11);

    printf("Sensor Setup complete\n");

  // call own functions
  bool isButton1pressedOnce();
  void barSingleGas();
  float voltageToPPM();



}

void loop() {
    // readout of Sensors
    int adc_reading1 = adc1_get_raw(Sensor1_ADC_CHANNEL);
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
    StaticJsonDocument<512> doc;

    //doc["battery_chg"] = true;
    doc["battery_pct"] = 84;
    doc["device_id"] = "99% von euch sind RATTEN!";
    doc["timestamp"] = "Sun, 20 Oct 2024 15:30:00 GMT";

    JsonArray sensors = doc.createNestedArray("sensors");

    JsonObject mq3 = sensors.createNestedObject();
    mq3["type"] = "Alcohol";
    mq3["val"] = ppmSensor1;

    JsonObject mq135 = sensors.createNestedObject();
    mq135["type"] = "Air Quality";
    mq135["val"] = ppmSensor2;

    JsonObject mq2 = sensors.createNestedObject();
    mq2["type"] = "Flammable Gas";
    mq2["val"] = ppmSensor3;

    JsonObject mq9 = sensors.createNestedObject();
    mq9["type"] = "Carbon Monoxide";
    mq9["val"] = ppmSensor4;

    JsonObject temperature = doc.createNestedObject("temperature");
    //temperature["type"] = "Temp";
    //temperature["unit"] = "°C";
    //temperature["val"] = 32.23;

    //JsonObject status = doc.createNestedObject("status");
    //status["alarm"] = true;
    //status["fault"] = true;

    // JSON-Data to chars
    char json_string[512];
    serializeJson(doc, json_string, sizeof(json_string));
    printf("%s\n", json_string);

    // renewing Display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);

  //---------------------- Button testing
    currentButten1State = digitalRead(BUTTON_PIN1);
  //----------------------

  switch(displayCase) {
	  case 0: 
      display.setCursor(0, 0);
      display.printf("MQ-3: %.2f ppm\n", ppmSensor1);
      barSingleGas(voltage1, 5.0 , 6 , 110 , 0, 7);

      display.setCursor(0, 14);
      display.printf("MQ-135: %.2f ppm\n", ppmSensor2);
      barSingleGas(voltage2, 5.0 , 6 , 110 , 0, 21);

      display.setCursor(0, 28);
      display.printf("MQ-2: %.2f ppm\n", ppmSensor3);
      barSingleGas(voltage2, 5.0 , 6 , 110 , 0, 35);

      display.setCursor(0, 42);
      display.printf("MQ-9: %.2f ppm\n", ppmSensor4);
      barSingleGas(voltage2, 5.0 , 6 , 110 , 0, 49);

      display.display();
      break;

  	case 1: 
      display.setCursor(0, 0);
      display.printf("MQ-3: %.2f ppm\n", ppmSensor1);
      display.printf("GAS: ----");
      barSingleGas(voltage1,sensorSupplyVoltage, 20 , 120 , 4, 30);
      display.display();
      break;

	  case 2:
      display.setCursor(0, 0); 
      display.printf("MQ-135: %.2f ppm\n", ppmSensor2);
      display.printf("GAS: ----");
      barSingleGas(voltage2,sensorSupplyVoltage, 20 , 120 , 4, 30);
      display.display();
      break;

    case 3:
      display.setCursor(0, 0); 
      display.printf("MQ-2: %.2f ppm\n", ppmSensor3);
      display.printf("GAS: ----");
      barSingleGas(voltage3,sensorSupplyVoltage, 20 , 120 , 4, 30);
      display.display();
      break;

    case 4:
      display.setCursor(0, 0); 
      display.printf("MQ-9: %.2f ppm\n", ppmSensor4);
      display.printf("GAS: ----");
      barSingleGas(voltage4,sensorSupplyVoltage, 20 , 120 , 4, 30);
      display.display();
      break;

    case 5:
      display.setCursor(0, 0);
      display.printf("Battery: --%\n");
      display.printf("      ALERT at:\n");
      display.printf("Gas 1: %.2fppm\n", alertPPMsensor1);
      display.printf("Gas 2: %.2fppm\n", alertPPMsensor2);
      display.printf("Gas 3: %.2fppm\n", alertPPMsensor3);
      display.printf("Gas 4: %.2fppm\n", alertPPMsensor4); 
      break;

    case 6:
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.printf("LoRa Connection Data\n");
      display.setTextSize(2);
      display.printf("RSSI: %i\n", rssiLoraDummy);
      display.printf("SNR:  %.1f \n", snrLoraDummy);
      display.display(); 
      break;

	  default: 
      display.setCursor(0, 0);
      display.printf("Buttonstate: %i \n", currentButten1State);
      display.printf("Case: %i \n", displayCase);
      display.printf("reset needed!");
      display.display();
      break;
}


    

    if(isButton1pressedOnce() == true){
      displayCase++;
      if(displayCase == 7){
        displayCase = 0;
      }
    }

    display.display();

    vTaskDelay(20 / portTICK_PERIOD_MS); // waiting
}

void app_main() {
    setup();
    while (true) {
        loop();
    }
}


//own functions
bool isButton1pressedOnce(){
  if(digitalRead(BUTTON_PIN1) == 0 && lastButten1State == 1){
    lastButten1State = 0;
    return true;
  }
  else{
    if(digitalRead(BUTTON_PIN1) == 1){
      lastButten1State = 1;
    }
    return false;
  }
}

void barSingleGas(float liveSensorVoltage,float maxVoltage,int16_t barHeight, int16_t barWidth, int16_t barStartX, int16_t barStartY){

  //how much of the bar needs to be filled out
  int filledWidth = (liveSensorVoltage / maxVoltage) * barWidth;

  // borderRecktangel
  display.drawRect(barStartX, barStartY, barWidth, barHeight, SH110X_WHITE);

  // fill out bar 2
  display.fillRect(barStartX, barStartY, filledWidth, barHeight, SH110X_WHITE);
}


/*
  float liveSensorVoltage
  float VCC Voltage supplied to the Sensor
  float Rl  Internal Resitor of the Sensor
  float R0  resitance of the calibrationResitor
  float m logarithmic curve specific for the sensor (in Datashee somtimes "slope or alpha") otherwise calculate with datasheet:
  float b a constant that shows the gas concentration when the sensor's resistance ratio Rs/R0​ is 1 for a specific gas(b = log(ppm fo a Known Rs/R0) - m*log(Rs/R0))
*/
float voltageToPPM(float liveSensorVoltage, float VCC, float Rl, float R0, float m, float b){


  float Rs = Rl * (VCC - liveSensorVoltage) / liveSensorVoltage;
  
  float ratioRsR0 = Rs / R0;

  // ppm = 10^((log(Rs/R0)-b)/m) 
  float ppm = pow(10, (log10(ratioRsR0) - b) /m);

  return ppm;
}

