#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"
#include "station.h"
#include "tasks.h"
#include "webSocket.h"





void app_main() 
{
    pca9548 *pca;
    s13683 *rgb;
    tps61165 *tps;

    rgb_device device = {
      .rgb = &rgb, .pca = &pca, .tps = &tps,
      .device_connected = false,
      .start_calibration =  false,
      .start_measures = false
    }; 

    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Connect esp32 to a wifi network (station) */
    printf("[main.c]: ESP_WIFI_MODE_STA\n");
    wifi_init_sta();

    /* I2C master initializaction */
    ret = pca9548_init();
    if (ret == ESP_OK) printf("[main.c]: pca9548 i2c comunication success\n");
    else if (ret == ESP_ERR_INVALID_ARG) printf("[main.c]: parameter error\n");
    else printf("[main.c]: driver install error\n");

    /* set configuration in all rgb sensors */

    /* Device Task */
    xTaskCreate(device_task, "measures_task", 8192, &device, 5, NULL);

    /* MQQT Task */
    //xTaskCreate(mqtt_task, "mqtt_task", 8192, &device, 5, NULL);

}




