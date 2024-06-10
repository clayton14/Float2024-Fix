#include <stdio.h>
#include <freertos/FreeRTOS.h>

#include "esp_log.h"
#include "webserver.h"
#include "config.h"

//Tag for logging
static const char *TAG = "main";


void app_main(void)
{

    void *param = NULL; 
    TaskHandle_t stepper_init_task = NULL;
    TaskHandle_t ws_task = NULL;

    xTaskCreate(ws_run, "WEBSERVER", 3584, param, 1, &ws_task);
    // xTaskCreate(stepper_init, "STEPPER", 3584, param, 1, &stepper_init_task);

    configASSERT(ws_task);
    //configASSERT(stepper_init_task);
    while(1)
    vTaskDelay(10);
    
// TODO impliment pressure sensor https://github.com/bluerobotics/BlueRobotics_MS5837_Library/blob/master/src/MS5837.cpp
// TODO log and save data to CSV file 
// TODO add timer and interrupts 
// TODO Start timer when launch request is made
// send data to website

}
