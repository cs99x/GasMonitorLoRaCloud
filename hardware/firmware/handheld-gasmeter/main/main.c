#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

// Definiere den ADC-Kanal, der mit dem MQ-7 verbunden ist
#define MQ7_ADC_CHANNEL ADC1_CHANNEL_6 // GPIO34

void app_main(void) {
    // Konfiguration des ADC
    adc1_config_width(ADC_WIDTH_BIT_12);          // 12-Bit-Auflösung
    adc1_config_channel_atten(MQ7_ADC_CHANNEL, ADC_ATTEN_DB_11); // 11 dB für maximale Spannungsreichweite

    printf("MQ-7 Gassensor-Test startet...\n");

    while (1) {
        // Lese die Spannung am MQ-7-Sensor
        int adc_reading = adc1_get_raw(MQ7_ADC_CHANNEL); // ADC-Rohwert (0 - 4095)
        float voltage = (adc_reading / 4095.0) * 3.3;    // Spannung in Volt (ESP32 arbeitet mit 3.3V)

        // Ausgabe der Rohwerte und Spannung
        printf("ADC-Rohwert: %d, Spannung: %.2f V\n", adc_reading, voltage);

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Warte 1 Sekunde
    }
}
