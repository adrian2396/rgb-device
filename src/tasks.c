#include "tasks.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

SemaphoreHandle_t print_mux = NULL;

void pca9548_task(pca9548_switch * arg){

    int ret;
    pca9548_switch sw = *(pca9548_switch*) arg;
    while (1) {
        sw.write_channels = 1;
        ret = pca9548_set_channels(I2C_MASTER_NUM, sw.write_channels);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("[tasks]: I2C Timeout write channels\n");
        } else if (ret == ESP_OK) {
            printf("[pca9548.c]: set actual channels:  "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(sw.write_channels));
            printf("\n");
        } else {
            printf("[tasks.c]:  No ack, pca9538 not connected...skip... error : %s\n", esp_err_to_name(ret));
        }
        vTaskDelay(1000);
        ret = pca9548_get_channels(I2C_MASTER_NUM, &sw.read_channels);
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

void s13673_task(s13683_sensor * arg)
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
        ret = s13683_is_operated(I2C_MASTER_NUM);
        vTaskDelay(4000/portTICK_PERIOD_MS);
        ret = s13683_read_data(I2C_MASTER_NUM, &rgb);
        xSemaphoreTake(print_mux, portMAX_DELAY);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("[tasks]: I2C Timeout");
        } else if (ret == ESP_OK) {
            /* Get data in 16 bits */
            rgb.data[0] = rgb.read_data[3] | rgb.read_data[2] << 8;
            rgb.data[1] = rgb.read_data[5] | rgb.read_data[4] << 8;
            rgb.data[2] = rgb.read_data[7] | rgb.read_data[6] << 8;
            rgb.data[3] = rgb.read_data[9] | rgb.read_data[8] << 8;
            /* Print the data */
            printf("***************************************\n");
            printf("* RGB_TASK  MASTER READ S13673 SENSOR *\n");
            printf("***************************************\n");
            printf("*   0x01   :   0x%02x   -----   %03d     *\n", rgb.read_data[0], rgb.read_data[0]);
            printf("*   0x02   :   0x%02x   -----   %03d     *\n", rgb.read_data[1], rgb.read_data[1]);
            printf("*   RRD_H  :   0x%02x   -----   %03d     *\n", rgb.read_data[2], rgb.read_data[2]);
            printf("*   RED_L  :   0x%02x   -----   %03d     *\n", rgb.read_data[3], rgb.read_data[3]);
            printf("*   GREEN_H:   0x%02x   -----   %03d     *\n", rgb.read_data[4], rgb.read_data[4]);
            printf("*   GREEN_L:   0x%02x   -----   %03d     *\n", rgb.read_data[5], rgb.read_data[5]);
            printf("*   BLUE_H :   0x%02x   -----   %03d     *\n", rgb.read_data[6], rgb.read_data[6]);
            printf("*   BLUE_L :   0x%02x   -----   %03d     *\n", rgb.read_data[7], rgb.read_data[7]);
            printf("*   CH_H   :   0x%02x   -----   %03d     *\n", rgb.read_data[8], rgb.read_data[8]);
            printf("*   CH_L   :   0x%02x   -----   %03d     *\n", rgb.read_data[9], rgb.read_data[9]);
            printf("***************************************\n");
            printf("*   RRD_H           "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb.read_data[2]));
            printf("          *\n");
            printf("*   RED_L           "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb.read_data[3]));
            printf("          *\n");
            printf("*   GREEN_H         "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb.read_data[4]));
            printf("          *\n");
            printf("*   GREEN_L         "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb.read_data[5]));
            printf("          *\n");
            printf("*   BLUE_H          "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb.read_data[6]));
            printf("          *\n");
            printf("*   BLUE_L          "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb.read_data[7]));
            printf("          *\n");
            printf("*   CH_H            "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb.read_data[8]));
            printf("          *\n");
            printf("*   CH_L            "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb.read_data[9]));
            printf("          *\n");
            printf("***************************************\n");
            printf("*   RED   :    %05d                  *\n",rgb.data[0]);
            printf("*   GREEN :    %05d                  *\n",rgb.data[1]);
            printf("*   BLUE  :    %05d                  *\n",rgb.data[2]);
            printf("*   CH    :    %05d                  *\n",rgb.data[3]);
            printf("***************************************\n");
            printf("***************************************\n");
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