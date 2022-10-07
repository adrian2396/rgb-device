#include "s13683.h"

esp_err_t s13683_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        // .clk_flags = 0,          /*!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here. */
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

esp_err_t s13683_is_operated(i2c_port_t i2c_num) {
    int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                              /* start condition */
    i2c_master_write_byte(cmd, 0x54, ACK_CHECK_EN);                     /* 01010100 => 0x54 (address call + write bit) */
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);                     /* 00000000 => 0x00 (adress call) */    
    i2c_master_write_byte(cmd, 0x84, ACK_CHECK_EN);                     /* 10001001 => 0x84 (Sleep, Reset and manual setting)*/
    i2c_master_write_byte(cmd, 0x0C, ACK_CHECK_EN);                     /* Manual Timing register */
    i2c_master_write_byte(cmd, 0x30, ACK_CHECK_EN);                     /* Manual Timing register */

    i2c_master_start(cmd);                                              /* restart condition */
    i2c_master_write_byte(cmd, 0x54, ACK_CHECK_EN);                     /* 01010100 => 0x54 (address call + write bit) */
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);                     /* 00000000 => 0x00 (address call)*/
    i2c_master_write_byte(cmd, 0x04, ACK_CHECK_EN);                     /* 00001001 => 0x09 (ADC reset dis, bus release, 1,4 ms)*/
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    return ret;

}
esp_err_t s13683_read_data(i2c_port_t i2c_num, s13683_sensor *arg){
    int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);                                           /* start condition */
    i2c_master_write_byte(cmd,0x54, ACK_CHECK_EN);                   /* 01010100 => 0x54 */

    i2c_master_start(cmd);                                           /* restart condition */
    i2c_master_write_byte(cmd, 0x55, ACK_CHECK_EN);                  /* 01010101 => 0x55  (address call + read bit) */
    i2c_master_read_byte(cmd, &arg->read_data[0], ACK_VAL);          /* Manual timing register (0x01) */
    i2c_master_read_byte(cmd, &arg->read_data[1], ACK_VAL);          /* Manual timing register (0x02) */
    i2c_master_read_byte(cmd, &arg->read_data[2], ACK_VAL);          /* RED_H READ */
    i2c_master_read_byte(cmd, &arg->read_data[3], ACK_VAL);          /* RED_L READ */
    i2c_master_read_byte(cmd, &arg->read_data[4], ACK_VAL);          /* GREEN_H READ */
    i2c_master_read_byte(cmd, &arg->read_data[5], ACK_VAL);          /* GREEN_L READ */
    i2c_master_read_byte(cmd, &arg->read_data[6], ACK_VAL);          /* BLUE_H READ */
    i2c_master_read_byte(cmd, &arg->read_data[7], ACK_VAL);          /* BLUE_L READ */
    i2c_master_read_byte(cmd, &arg->read_data[8], ACK_VAL);          /* CH_H READ */
    i2c_master_read_byte(cmd, &arg->read_data[9], NACK_VAL);         /* CH_L READ */
    i2c_master_stop(cmd);
    
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}
void read_data_to_hex(s13683_sensor *rgb){
    
    /* Get data in 16 bits */
    rgb->data[0] = rgb->read_data[3] | rgb->read_data[2] << 8;
    rgb->data[1] = rgb->read_data[5] | rgb->read_data[4] << 8;
    rgb->data[2] = rgb->read_data[7] | rgb->read_data[6] << 8;
    rgb->data[3] = rgb->read_data[9] | rgb->read_data[8] << 8;
    /* Print the data */
    printf("***************************************\n");
    printf("* RGB_TASK  MASTER READ S13673 SENSOR *\n");
    printf("***************************************\n");
    printf("*   0x01   :   0x%02x   -----   %03d     *\n", rgb->read_data[0], rgb->read_data[0]);
    printf("*   0x02   :   0x%02x   -----   %03d     *\n", rgb->read_data[1], rgb->read_data[1]);
    printf("*   RRD_H  :   0x%02x   -----   %03d     *\n", rgb->read_data[2], rgb->read_data[2]);
    printf("*   RED_L  :   0x%02x   -----   %03d     *\n", rgb->read_data[3], rgb->read_data[3]);
    printf("*   GREEN_H:   0x%02x   -----   %03d     *\n", rgb->read_data[4], rgb->read_data[4]);
    printf("*   GREEN_L:   0x%02x   -----   %03d     *\n", rgb->read_data[5], rgb->read_data[5]);
    printf("*   BLUE_H :   0x%02x   -----   %03d     *\n", rgb->read_data[6], rgb->read_data[6]);
    printf("*   BLUE_L :   0x%02x   -----   %03d     *\n", rgb->read_data[7], rgb->read_data[7]);
    printf("*   CH_H   :   0x%02x   -----   %03d     *\n", rgb->read_data[8], rgb->read_data[8]);
    printf("*   CH_L   :   0x%02x   -----   %03d     *\n", rgb->read_data[9], rgb->read_data[9]);
    printf("***************************************\n");
    printf("*   RRD_H           "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb->read_data[2]));
    printf("          *\n");
    printf("*   RED_L           "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb->read_data[3]));
    printf("          *\n");
    printf("*   GREEN_H         "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb->read_data[4]));
    printf("          *\n");
    printf("*   GREEN_L         "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb->read_data[5]));
    printf("          *\n");
    printf("*   BLUE_H          "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb->read_data[6]));
    printf("          *\n");
    printf("*   BLUE_L          "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb->read_data[7]));
    printf("          *\n");
    printf("*   CH_H            "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb->read_data[8]));
    printf("          *\n");
    printf("*   CH_L            "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rgb->read_data[9]));
    printf("          *\n");
    printf("***************************************\n");
    printf("*   RED   :    %05d                  *\n",rgb->data[0]);
    printf("*   GREEN :    %05d                  *\n",rgb->data[1]);
    printf("*   BLUE  :    %05d                  *\n",rgb->data[2]);
    printf("*   CH    :    %05d                  *\n",rgb->data[3]);
    printf("***************************************\n");
    printf("***************************************\n");

    char hexcol[16];

    snprintf(hexcol, sizeof hexcol, "%02x%02x%02x", rgb->read_data[0], rgb->read_data[1], rgb->read_data[1]);
}