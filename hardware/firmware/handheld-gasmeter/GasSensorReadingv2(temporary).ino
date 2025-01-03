#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

// Definiere den ADC-Kanal, der mit dem MQ-7 verbunden ist
#define MQ3_ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34
#define MQ135_ADC_CHANNEL ADC1_CHANNEL_7 // GPIO35
#define MQ2_ADC_CHANNEL ADC1_CHANNEL_4 // GPIO32
#define MQ9_ADC_CHANNEL ADC1_CHANNEL_5 // GPIO33

void setup() {
  // Konfiguration MQ3
    adc1_config_width(ADC_WIDTH_BIT_12);          // 12-Bit-Auflösung
    adc1_config_channel_atten(MQ3_ADC_CHANNEL, ADC_ATTEN_DB_11); // 11 dB für maximale Spannungsreichweite

    printf("MQ-3 Gassensor-Test startet...\n");
    // Code, der einmalig beim Start ausgeführt wird

    //---------------------

    // Konfiguration MQ 135
    adc1_config_width(ADC_WIDTH_BIT_12);          // 12-Bit-Auflösung
    adc1_config_channel_atten(MQ135_ADC_CHANNEL, ADC_ATTEN_DB_11); // 11 dB für maximale Spannungsreichweite

    printf("MQ-135 Gassensor-Test startet...\n");
    // Code, der einmalig beim Start ausgeführt wird

    //---------------------

    // Konfiguration MQ 2
    adc1_config_width(ADC_WIDTH_BIT_12);          // 12-Bit-Auflösung
    adc1_config_channel_atten(MQ2_ADC_CHANNEL, ADC_ATTEN_DB_11); // 11 dB für maximale Spannungsreichweite

    printf("MQ-2 Gassensor-Test startet...\n");
    // Code, der einmalig beim Start ausgeführt wird

    //---------------------

    // Konfiguration MQ 9
    adc1_config_width(ADC_WIDTH_BIT_12);          // 12-Bit-Auflösung
    adc1_config_channel_atten(MQ9_ADC_CHANNEL, ADC_ATTEN_DB_11); // 11 dB für maximale Spannungsreichweite

    printf("MQ-9 Gassensor-Test startet...\n");
    // Code, der einmalig beim Start ausgeführt wird
}

void loop() {
    // Lese die Spannung am MQ-7-Sensor
        int adc_reading1 = adc1_get_raw(MQ3_ADC_CHANNEL); // ADC-Rohwert (0 - 4095)
        float voltage1 = (adc_reading1 / 4095.0) * 3.3;    // Spannung in Volt (ESP32 arbeitet mit 3.3V)

        // Ausgabe der Rohwerte und Spannung
        printf("ADC-Rohwert MQ-7: %d, Spannung: %.2f V\n", adc_reading1, voltage1);

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Warte 1 Sekunde

      //------------------


        int adc_reading2 = adc1_get_raw(MQ135_ADC_CHANNEL); // ADC-Rohwert (0 - 4095)
        float voltage2 = (adc_reading2 / 4095.0) * 3.3;    // Spannung in Volt (ESP32 arbeitet mit 3.3V)

        // Ausgabe der Rohwerte und Spannung
        printf("ADC-Rohwert MQ-135: %d, Spannung: %.2f V\n", adc_reading2, voltage2);

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Warte 1 Sekunde

      //------------------



        int adc_reading3 = adc1_get_raw(MQ2_ADC_CHANNEL); // ADC-Rohwert (0 - 4095)
        float voltage3 = (adc_reading3 / 4095.0) * 3.3;    // Spannung in Volt (ESP32 arbeitet mit 3.3V)

        // Ausgabe der Rohwerte und Spannung
        printf("ADC-Rohwert MQ-2: %d, Spannung: %.2f V\n", adc_reading3, voltage3);

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Warte 1 Sekunde

      //------------------


        int adc_reading4 = adc1_get_raw(MQ9_ADC_CHANNEL); // ADC-Rohwert (0 - 4095)
        float voltage4 = (adc_reading4 / 4095.0) * 3.3;    // Spannung in Volt (ESP32 arbeitet mit 3.3V)

        // Ausgabe der Rohwerte und Spannung
        printf("ADC-Rohwert MQ-9: %d, Spannung: %.2f V\n", adc_reading4, voltage4);

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Warte 1 Sekunde
}
