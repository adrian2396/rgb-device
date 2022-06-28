#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "station.h"
#include "tasks.h"

rgb_sensor *rgb;

void app_main() 
{
    
    /* Connect esp32 to a wifi network (station) */
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    printf("[main]: ESP_WIFI_MODE_STA\n");
    wifi_init_sta();

    /* I2C master initializaction */
    ESP_ERROR_CHECK(i2c_master_init());

    /* MQQT start and conected to AWS broker */

    /* S13683 Task Created */
    xTaskCreate(s13673_task, "s13673_task_0", 1024 * 2, &rgb, 10, NULL);
    
    /* Oled Task Created */

    /* Buttons Task Created */
}




