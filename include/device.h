#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "pca9548.h"
#include "s13683.h"
#include "tps61165.h"


/* NOTE: this file is used to measures the real RGB values
   & to get the duty values forcalibrate the RGB sensors */

typedef struct Device
{
    s13683 *rgb;
    pca9548 *pca;
    tps61165 *tps;

    int device_connected;
    int start_calibration;
    int start_measures;

} rgb_device;

void device_measures(rgb_device *device);
void device_calibration(rgb_device *device);




#endif /* __DEVICE_H__ */