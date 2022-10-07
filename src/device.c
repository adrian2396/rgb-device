#include "device.h"

void device_measures(rgb_device *device){
    for (int i = 0; i < N_RGB_SENSORS; i++)
    {
        if (device->rgb->active_sensors[i] == true)
        {
            device->pca->write_channels = i;
            pca9548_set_channels(I2C_MASTER_NUM, &device->pca);

            /* start measure */
            s13683_read_data(I2C_MASTER_NUM, &device->rgb);

            /* convert data to HEX (#XXXXXX) */

        }
    }
    /* send data to server */

}
void device_calibration(rgb_device *device){
    int n_sensors[N_RGB_SENSORS +1] = {0, 7, 8, 6, 5, 1, 3, 2, 4};
    /* calibrate column A (4 wells) */
    for (int i = 1; i <= N_RGB_SENSORS/2; i++)
    {
        if (device->rgb->active_sensors[i] == true)
        {
            device->pca->write_channels = n_sensors[i];
            pca9548_set_channels(I2C_MASTER_NUM, &device->pca);

            for (int duty = 0; duty < TPS61165_MAX_DUTY; duty++)
            {
                tps61165_set_duty(0, 0, duty);
                /* read */
                int ret;
                ret = s13683_read_data(I2C_MASTER_NUM, &device->rgb);
                if (ret == ESP_ERR_TIMEOUT) {
                    printf("[tasks]: I2C Timeout");
                } else if (ret == ESP_OK) {
                    /* check if read value == 0xFFFFFF */
                    if(true){
                        device->tps->duty[i][0] = duty;
                        break;
                    }
                }
            }
        }
    }

    /* calibrate column B (4 wells) */
    for (int i = (N_RGB_SENSORS/2 +1); i <= N_RGB_SENSORS; i++)
    {
        if (device->rgb->active_sensors[i] == true)
        {
            device->pca->write_channels = n_sensors[i];
            pca9548_set_channels(I2C_MASTER_NUM, &device->pca);

            for (int duty = 0; duty < TPS61165_MAX_DUTY; duty++)
            {
                if (i%2 == false) tps61165_set_duty(0, 0, duty);
                else tps61165_set_duty(0, 0, duty);
                
                /* read */
                int ret;
                ret = s13683_read_data(I2C_MASTER_NUM, &device->rgb);
                if (ret == ESP_ERR_TIMEOUT) {
                    printf("[tasks]: I2C Timeout");
                } else if (ret == ESP_OK) {
                    /* check if read value == 0xFFFFFF */
                    if(true){
                        device->tps->duty[0][i] = duty;
                        break;
                    }
                }
            }
        }
    }

}