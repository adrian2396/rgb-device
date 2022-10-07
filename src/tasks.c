#include "tasks.h"


SemaphoreHandle_t print_mux = NULL;

TaskFunction_t device_task(rgb_device * device){
    rgb_device mc = *(rgb_device*) device;
    while (1) {
        if(mc.start_calibration == "start_measures"){
            device_measures(&mc);
            mc.start_calibration
            
        }
        if (mc.start_calibration == "start_calibration"){
            device_calibration(&mc);
        }
    }   
    
}

TaskFunction_t mqtt_task(rgb_device * device){
    vTaskDelay(100);
    while (true)
    {
        /* code */
    }
    
}

