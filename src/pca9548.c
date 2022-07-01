#include "pca9548.h"

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

esp_err_t pca9548_init(void)
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

esp_err_t pca9548_set_channels(i2c_port_t i2c_num, uint8_t channels_write){
    int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();           
    i2c_master_start(cmd);                                                          /* start condition */
    i2c_master_write_byte(cmd, (PCA9548_ADDRESS << 1) | WRITE_BIT, ACK_CHECK_EN);   /* 0x00 slave address */
    i2c_master_write_byte(cmd, channels_write, ACK_CHECK_DIS);                      /* set active channel */
    i2c_master_stop(cmd);                                                           /* stop condition */

    ret = i2c_master_cmd_begin(i2c_num, cmd, 10000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }
    
    return ret;
}

esp_err_t pca9548_get_channels(i2c_port_t i2c_num, uint8_t *channels_read){
    int ret;

    uint8_t ch = (uint8_t) *channels_read;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();           
    i2c_master_start(cmd);                                                          /* start condition */
    i2c_master_write_byte(cmd, (PCA9548_ADDRESS << 1) | READ_BIT, ACK_CHECK_EN);    /* 0x00 slave address */
    i2c_master_read_byte(cmd, &ch, ACK_VAL);                             /* read active channels */
    i2c_master_stop(cmd);                                                           /* stop condition */

    ret = i2c_master_cmd_begin(i2c_num, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    

    return ret;
}