#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

void app_main(void)
{
    printf("Hello, World! Der ESP32 ist bereit!\n");

    // Unendlich-Schleife
    while (1) {
        printf("Hello World\n");
    
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}