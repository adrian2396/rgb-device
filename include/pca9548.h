#ifndef __PCA9548_H__
#define __PCA9548_H__

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "sdkconfig.h"

#define I2C_MASTER_SCL_IO           22                  /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO           21                  /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM              0                   /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ          300000              /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                   /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                   /*!< I2C master doesn't need buffer */

#define PCA9548_ADDRESS             0X70

#define WRITE_BIT                   0x00                /*!< I2C master write */
#define READ_BIT                    0x01                /*!< I2C master read */
#define ACK_CHECK_EN                0x1                 /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS               0x0                 /*!< I2C master will not check ack from slave */
#define ACK_VAL                     0                   /*!< I2C ack value */
#define NACK_VAL                    1   

typedef struct pca9548
{
    uint8_t write_channels;
    uint8_t read_channels;
} pca9548_switch;

/**
 *  @brief i2c master initialization
 */

esp_err_t pca9548_init(void);

/**
 * @brief Switch channels
 * @brief Operation to set channels
 * 
 *   _________________________________________________________________
 *  | start | slave_addr + wr_bit + ack | set channel  + nack | stop |
 *   _________________________________________________________________
 *
 * @param i2c_num Device descriptor
 * @param channels Channel flags, combination of TCA9548_CHANNELn
 * @return `ESP_OK` on success
 */
esp_err_t pca9548_set_channels(i2c_port_t i2c_num, uint8_t channels_write);


/**
 * @brief Read current channels configuration
 *  @brief Operation to get channels
 * 
 *    ________________________________________________________________
 *   | start | slave_addr + rd_bit + ack | get channel  + nack | stop |
 *    ________________________________________________________________
 * 
 * @param i2c_num Device descriptor
 * @param[out] channels Channel flags, combination of TCA9548_CHANNELn
 * @return `ESP_OK` on success
 */
esp_err_t pca9548_get_channels(i2c_port_t i2c_num, uint8_t *channels_read);

#endif /* __PCA9548_H__ */

