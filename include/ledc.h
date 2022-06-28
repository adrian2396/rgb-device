#ifndef __LEDC_H__
#define __LEDC_H__

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5) 
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT 
#define LEDC_DUTY               (4095) 
#define LEDC_FREQUENCY          (5000) 

static void ledc_init(void);

#endif /* __LEDC_H__ */