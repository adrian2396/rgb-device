#include "tasks.h"

SemaphoreHandle_t print_mux = NULL;

void s13673_task(rgb_sensor * arg)
{
    print_mux = xSemaphoreCreateMutex();
    int ret;
    rgb_sensor rgb = *(rgb_sensor*) arg;
    int cnt = 0;
    while (1) {
        printf("[tasks]: RGB_TASK test cnt: %d\n", cnt++);
        ret = i2c_master_sensor_test(I2C_MASTER_NUM, &rgb);
        xSemaphoreTake(print_mux, portMAX_DELAY);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("[tasks]: I2C Timeout");
        } else if (ret == ESP_OK) {
            printf("***********************************\n");
            printf("RGB_TASK  MASTER READ S13673 SENSOR\n");
            printf("***********************************\n");
            
            printf("RRD_H  :   %02x   -----   %03d\n", rgb.data[0], rgb.data[0]);
            printf("RED_L  :   %02x   -----   %03d\n", rgb.data[1], rgb.data[1]);
            printf("GREEN_H:   %02x   -----   %03d\n", rgb.data[2], rgb.data[2]);
            printf("GREEN_L:   %02x   -----   %03d\n", rgb.data[3], rgb.data[3]);
            printf("BLUE_H :   %02x   -----   %03d\n", rgb.data[4], rgb.data[4]);
            printf("BLUE_L :   %02x   -----   %03d\n", rgb.data[5], rgb.data[5]);
            printf("CH_H   :   %02x   -----   %03d\n", rgb.data[6], rgb.data[6]);
            printf("CH_L   :   %02x   -----   %03d\n", rgb.data[7], rgb.data[7]);
        } else {
            printf("[tasks]: %s: No ack, sensor not connected...skip...\n", esp_err_to_name(ret));
        }
        xSemaphoreGive(print_mux);
        vTaskDelay(10000/portTICK_PERIOD_MS);
        ++cnt;
    }
    vSemaphoreDelete(print_mux);
    vTaskDelete(NULL);
}