#include "tasks.h"


SemaphoreHandle_t print_mux = NULL;

TaskFunction_t pca9548_task(pca9548_switch * pca){
    int ret;
    pca9548_switch sw = *(pca9548_switch*) pca;
    while (1) {
        sw.write_channels = 0x01;
        ret = pca9548_set_channels(I2C_MASTER_NUM, &sw);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("[tasks]: I2C Timeout write channels\n");
        } else if (ret == ESP_OK) {
            printf("[pca9548.c]: set actual channels:  "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(sw.write_channels));
            printf("\n");
        } else {
            printf("[tasks.c]:  No ack, pca9538 not connected...skip... error : %s\n", esp_err_to_name(ret));
        }
        vTaskDelay(1000);
        ret = pca9548_get_channels(I2C_MASTER_NUM, &sw);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("[tasks]: I2C Timeout get channels\n");
        } else if (ret == ESP_OK) {
            printf("[pca9548.c]: get actual channels:  "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(sw.read_channels));
            printf("\n");

        } else {
            printf("[tasks.c]:  No ack, pca9538 not connected...skip... error: %s\n", esp_err_to_name(ret));
        }
        vTaskDelay(1000);
    }
    
}

TaskFunction_t s13673_task(s13683_sensor * arg)
{
    print_mux = xSemaphoreCreateMutex();
    int ret;
    s13683_sensor rgb = *(s13683_sensor*) arg;
    int cnt = 0;
    while (1) {
        printf("***************************************\n");
        if (cnt < 10) printf("*   [tasks.c]: RGB_TASK test cnt: %d     *\n", cnt++);
        else if (cnt >= 10 && cnt < 100) printf("*  [tasks.c]: RGB_TASK test cnt: %d     *\n", cnt++);
        else if (cnt >= 100 && cnt < 1000) printf("*  [tasks.c]: RGB_TASK test cnt: %d     *\n", cnt++);
        ret = s13683_init();
        ret = s13683_is_operated(I2C_MASTER_NUM);
        vTaskDelay(4000/portTICK_PERIOD_MS);
        ret = s13683_read_data(I2C_MASTER_NUM, &rgb);
        xSemaphoreTake(print_mux, portMAX_DELAY);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("[tasks]: I2C Timeout");
        } else if (ret == ESP_OK) {
            read_data_to_hex(&rgb);
        } else {
            printf("[tasks.c]: %s: No ack, RGB sensor not connected...skip...\n", esp_err_to_name(ret));
        }
        xSemaphoreGive(print_mux);
        vTaskDelay(10000/portTICK_PERIOD_MS);
        ++cnt;
    }
    vSemaphoreDelete(print_mux);
    vTaskDelete(NULL);
}

TaskFunction_t tps61165_task(tps61165_driver *driver)
{
    while (1)
    {
        printf("i2c switch task");
    }
    
}

TaskFunction_t  http_task(void *pvParameters)
{
    while (1)
    {
        printf("http task");
    }
}
