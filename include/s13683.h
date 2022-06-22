#ifndef __S13683_H__
#define __S13683_H__

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

#define DATA_LENGTH                 10                   /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH              128                 /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000                /*!< delay time between different test items */

typedef struct s13683
{
    uint8_t read_data[DATA_LENGTH];
    uint16_t data[DATA_LENGTH - 6];
} rgb_sensor;

/**
 *  @brief i2c master initialization
 */
esp_err_t i2c_master_init(void);


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

esp_err_t i2c_master_sensor_test(i2c_port_t i2c_num, rgb_sensor *arg);




#endif /* __S13683_H__ */