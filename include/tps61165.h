#ifndef __TPS61165_H__
#define __TPS61165_H__

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#include "pca9548.h"
#include "s13683.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5) 
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT 
#define TPS61165_MAX_DUTY       255
#define LEDC_FREQUENCY          (5000) 

#define N_WELLS                 8
#define COLUMNS                 2
#define ROWS                    4

typedef struct TPS61165
{
    s13683 rgb;
    uint8_t duty[ROWS][COLUMNS];
} tps61165;

void tps61165_init(void);
void tps61165_set_duty();
void tps61165_calibration(ledc_mode_t speed_mode, ledc_channel_t channel,  ledc_channel_t channel_B, tps61165 driver);


#endif /* __TPS61165_H__ */