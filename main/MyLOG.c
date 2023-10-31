// https://iotespresso.com/how-to-check-free-memory-in-esp32/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include <stdio.h>
#include <inttypes.h>

#define  INCLUDE_xTaskDelayUntil 1

static const char *TAG1 = "LOG";

static void My_LOG_task(void *pvParameters){

TickType_t xLastWakeTime;
BaseType_t xWasDelayed;
const TickType_t xFrequency = 1000;
xLastWakeTime = xTaskGetTickCount ();

while (1)
{
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime,xFrequency);
    
    ESP_LOGI(TAG1, "[APP] Free memory: %ld bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG1, "1[APP] largest block MALLOC_CAP_EXEC: %d bytes", heap_caps_get_free_size(MALLOC_CAP_EXEC ));
    ESP_LOGI(TAG1, "2[APP] largest block MALLOC_CAP_32BIT: %d bytes", heap_caps_get_free_size(MALLOC_CAP_32BIT ));
    ESP_LOGI(TAG1, "3[APP] largest block MALLOC_CAP_8BIT: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT ));
    ESP_LOGI(TAG1, "4[APP] largest block MALLOC_CAP_DMA: %d bytes", heap_caps_get_free_size(MALLOC_CAP_DMA ));
   // ESP_LOGI(TAG1, "5[APP] largest block MALLOC_CAP_SPIRAM: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM ));
    ESP_LOGI(TAG1, "6[APP] largest block MALLOC_CAP_INTERNAL: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL ));
    ESP_LOGI(TAG1, "7[APP] largest block MALLOC_CAP_DEFAULT: %d bytes", heap_caps_get_free_size(MALLOC_CAP_DEFAULT ));
    // ESP_LOGI(TAG1, "8[APP] largest block MALLOC_CAP_IRAM_8BIT: %d bytes", heap_caps_get_free_size(MALLOC_CAP_IRAM_8BIT ));
    //  ESP_LOGI(TAG1, "9[APP] largest block MALLOC_CAP_RETENTION: %d bytes", heap_caps_get_free_size(MALLOC_CAP_RETENTION ));
    // ESP_LOGI(TAG1, "10[APP] largest block MALLOC_CAP_RTCRAM: %d bytes", heap_caps_get_free_size(MALLOC_CAP_RTCRAM ));
    // ESP_LOGI(TAG1, "11[APP] largest block MALLOC_CAP_INVALID: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INVALID ));
    }
}