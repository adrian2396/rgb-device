#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "sdkconfig.h"


#define I2C_MASTER_SCL_IO           22                  /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO           21                  /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM              0                   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          300000              /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                   /*!< I2C master doesn't need buffer */
#define I2C_SDA_PULL_UP             1
#define I2C_SCL_PULL_UP             1

#define S13683_SENSOR_ADDR          0X2A                /*!< slave address for BH1750 sensor */
#define S13683_CONTROL_BYTE         0X00                /*!< Operation mode */
#define S13683_RESET_DISABLE        0X89
#define S13683_RESET_RELEASE        0X09
#define S13673_OUTPUT_DATA_BYTE     0X03

#define WRITE_BIT                   0x00                /*!< I2C master write */
#define READ_BIT                    0x01                /*!< I2C master read */
#define ACK_CHECK_EN                0x1                 /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS               0x0                 /*!< I2C master will not check ack from slave */
#define ACK_VAL                     0                   /*!< I2C ack value */
#define NACK_VAL                    1                   /*!< I2C nack value */

#define DATA_LENGTH                 8                   /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH              128                 /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000                /*!< delay time between different test items */

SemaphoreHandle_t print_mux = NULL;

/**
 *  @brief Test code to operate on S13683 sensor
 *
 * 1. set operation mode.
 * ____________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | Calls control byte + ack  | ADC reset, standby disaable + ack |
 * --------|---------------------------|---------------------------|-----------------------------------|------
 * | start | slave_addr + wr_bit + ack | Calls control byte + ack  | ADC reset dis, bus release  + ack | stop |
 * ____________________________________________________________________________________________________________
 * 2. wait more than integration time
 * 3. read data
 * ___________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | Calls output byte + ack  | slave_addr + rd_bit + ack |
 * --------|---------------------------|--------------------------|---------------------------|
 */

static esp_err_t i2c_master_sensor_test(i2c_port_t i2c_num, uint8_t *data)
{
    int ret;
    uint8_t *p;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                              /* start condition */
    i2c_master_write_byte(cmd, 0x54, ACK_CHECK_EN);                     /* 01010100 => 0x54 */
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);                     /* 00000000 => 0x00 */    
    i2c_master_write_byte(cmd, 0x89, ACK_CHECK_EN);                     /* 10001001 => 0x89 (ADC reset, standby dis, 1,4 ms)*/

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                              /* restart condition */
    i2c_master_write_byte(cmd, 0x54, ACK_CHECK_EN);                     /* 01010100 => 0x54 */
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);                     /* 00000000 => 0x00 */
    i2c_master_write_byte(cmd, 0x09, ACK_CHECK_EN);                     /* 00001001 => 0x09 (ADC reset dis, bus release, 1,4 ms)*/
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(200/portTICK_PERIOD_MS);                              /* delay > integration time */
    
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                           /* start condition */
    i2c_master_write_byte(cmd,0x54, ACK_CHECK_EN);                   /* 01010100 => 0x54 */
    i2c_master_write_byte(cmd, 0x03, ACK_CHECK_EN);                  /* 00000011 => 0x03 (11 == 170,2 ms¿¿??)*/

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                           /* restart condition */
    i2c_master_write_byte(cmd, 0x55, ACK_CHECK_EN);                  /* 01010101 => 0x54 */
    i2c_master_read_byte(cmd, p = &data[0], ACK_VAL);                /* RED_H READ */
    i2c_master_read_byte(cmd, p = &data[1], ACK_VAL);                /* RED_L READ */
    i2c_master_read_byte(cmd, p = &data[2], ACK_VAL);                /* GREEN_H READ */
    i2c_master_read_byte(cmd, p = &data[3], ACK_VAL);                /* GREEN_L READ */
    i2c_master_read_byte(cmd, p = &data[4], ACK_VAL);                /* BLUE_H READ */
    i2c_master_read_byte(cmd, p = &data[5], ACK_VAL);                /* BLUE_L READ */
    i2c_master_read_byte(cmd, p = &data[6], ACK_VAL);                /* CH_H READ */
    i2c_master_read_byte(cmd, p = &data[7], NACK_VAL);               /* CH_L READ */
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 *  @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = 1,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = 1,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static void s13673_task(void * arg)
{
    int ret;
    uint32_t task_idx = (uint32_t)arg;
    uint8_t *data = (uint8_t *)malloc(DATA_LENGTH);
    int cnt = 0;
    while (1) {
        printf("TASK[%d] test cnt: %d\n", task_idx, cnt++);
        ret = i2c_master_sensor_test(I2C_MASTER_NUM, data);
        xSemaphoreTake(print_mux, portMAX_DELAY);
        if (ret == ESP_ERR_TIMEOUT) {
            printf("I2C Timeout");
        } else if (ret == ESP_OK) {
            printf("***********************************\n");
            printf("TASK[%d]  MASTER READ S13673 SENSOR\n", task_idx);
            printf("***********************************\n");
            printf("RRD_H  :   %02x   -----   %03d\n", data[0], data[0]);
            printf("RED_L  :   %02x   -----   %03d\n", data[1], data[1]);
            printf("GREEN_H:   %02x   -----   %03d\n", data[2], data[2]);
            printf("GREEN_L:   %02x   -----   %03d\n", data[3], data[3]);
            printf("BLUE_H :   %02x   -----   %03d\n", data[4], data[4]);
            printf("BLUE_L :   %02x   -----   %03d\n", data[5], data[5]);
            printf("CH_H   :   %02x   -----   %03d\n", data[6], data[6]);
            printf("CH_L   :   %02x   -----   %03d\n", data[7], data[8]);
        } else {
            printf("%s: No ack, sensor not connected...skip...\n", esp_err_to_name(ret));
        }
        xSemaphoreGive(print_mux);
        vTaskDelay(10000/portTICK_PERIOD_MS);
        ++cnt;
    }
    vSemaphoreDelete(print_mux);
    vTaskDelete(NULL);
}



void app_main() 
{
    print_mux = xSemaphoreCreateMutex();

    ESP_ERROR_CHECK(i2c_master_init());
    xTaskCreate(s13673_task, "i2c_test_task_0", 1024 * 2, (void *)0, 10, NULL);
}




