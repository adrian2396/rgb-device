#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"

#include "tasks.h"

rgb_sensor *rgb;


void app_main() 
{

    ESP_ERROR_CHECK(i2c_master_init());
    xTaskCreate(s13673_task, "i2c_test_task_0", 1024 * 2, &rgb, 10, NULL);
}




