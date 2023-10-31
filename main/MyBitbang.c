#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver\gpio.h"
#include "hal\gpio_types.h"

#define GPIO_1kHz GPIO_NUM_22
#define GPIO_En_Buff GPIO_NUM_17
#define GPIO_CS GPIO_NUM_21

//#define GPIO_1kHz GPIO_NUM_22


void My_bitbang(void *pvParameters){

gpio_set_direction(GPIO_1kHz, GPIO_MODE_OUTPUT);

gpio_set_direction(GPIO_En_Buff, GPIO_MODE_OUTPUT);
gpio_set_level(GPIO_En_Buff, 1);

TickType_t xLastWakeTime;
BaseType_t xWasDelayed;
const TickType_t xFrequency = 1;
xLastWakeTime = xTaskGetTickCount ();

while (1){
    xWasDelayed = xTaskDelayUntil(&xLastWakeTime,xFrequency);

    gpio_set_level(GPIO_1kHz, 1);

    gpio_set_level(GPIO_1kHz, 0);

}



}