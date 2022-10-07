#include "tps61165.h"



void tps61165_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, 
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void tps61165_set_duty(ledc_mode_t speed_mode, ledc_channel_t channel, uint32_t duty){
    ledc_set_duty(speed_mode, channel, duty);
    ledc_update_duty(speed_mode, channel);
}

void tps61165_calibration(ledc_mode_t speed_mode, ledc_channel_t channel_A, ledc_channel_t channel_B, tps61165 driver){
    
    int ret;
    int well = 0;

    /* calibrate column A (4 wells) */
    for (size_t i = 0; i < N_WELLS/4; i++)
    {
        well = i;
        for (size_t duty = 0; duty < TPS61165_MAX_DUTY; duty++)
        {
            tps61165_set_duty(speed_mode, channel_A, duty);
            /* read and check */
            ret = s13683_read_data(I2C_MASTER_NUM, &driver.rgb);
            if (ret == ESP_ERR_TIMEOUT) {
                printf("[tasks]: I2C Timeout");
            } else if (ret == ESP_OK) {
                //char hexcol[16];

                //snprintf(hexcol, sizeof hexcol, "%02x%02x%02x", rgb.read_data, rgb.read_data, rgb.read_data);
                if (true){
                    /* save duty value. This value saturate the RGB sensor */
                    driver.duty[well][0] = duty;
                    break;
                }
            }
        }
        
    }
    /* calibrate column B (4 wells) */
    for (size_t i = 0; i < N_WELLS/4; i++)
    {
        for (size_t duty = 0; duty < TPS61165_MAX_DUTY; duty++)
        {
            tps61165_set_duty(speed_mode, channel_B, duty);
            /* read and check */
            ret = s13683_read_data(I2C_MASTER_NUM, &driver.rgb);
            if (ret == ESP_ERR_TIMEOUT) {
                printf("[tasks]: I2C Timeout");
            } else if (ret == ESP_OK) {
                if (true){
                    /* save duty value. This value saturate the RGB sensor */
                    driver.duty[well][0] = duty;
                    break;
                }
            }
        }
    }
    

}


