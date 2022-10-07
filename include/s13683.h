#ifndef __S13683_H__
#define __S13683_H__

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

#define N_RGB_SENSORS               8

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

typedef struct s13683_sensor
{
    uint8_t read_data[DATA_LENGTH];     /* save raw data from rgb sensros (R, G, B, CH) */
    uint16_t data[N_RGB_SENSORS];       /* save hex data (max 8 sensor) */

    int start_measures;
    int start_calibration;

    int active_sensors[N_RGB_SENSORS];  /* save active sensors got from ui */
} s13683;

/**
 *  @brief i2c master initialization
 */
esp_err_t s13683_init(void);

/**
 *  @brief Set configuration to operate with sensor
 *
 * 1. set operation mode.
 * ____________________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | Calls control byte + ack  | ADC reset, standby disaable + ack |
 * --------|---------------------------|---------------------------|-----------------------------------|------
 * | start | slave_addr + wr_bit + ack | Calls control byte + ack  | ADC reset dis, bus release  + ack | stop |
 * ____________________________________________________________________________________________________________
 * 2. wait more than integration time
 */

esp_err_t s13683_is_operated(i2c_port_t i2c_num);

esp_err_t s13683_read_data(i2c_port_t i2c_num, s13683 *arg);
/**
 *  @brief Read data from slave
 * 1. read data
 * ___________________________________________________________________________________________
 * | start | slave_addr + wr_bit + ack | Calls output byte + ack  | slave_addr + rd_bit + ack |
 * --------|---------------------------|--------------------------|---------------------------|
 */

esp_err_t i2c_master_sensor_test(i2c_port_t i2c_num, s13683 *arg);


void read_data_to_hex(s13683 *arg);




#endif /* __S13683_H__ */