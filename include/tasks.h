#ifndef __TASKS_H__
#define __TASKS_H__

#include <string.h>
#include <stdint.h>

#include "pca9548.h"
#include "s13683.h"
#include "tps61165.h"
#include "device.h"
#include "client.h"

TaskFunction_t  device_task(rgb_device *device);
TaskFunction_t mqtt_task(rgb_device * device);


#endif /* __TASKS_H__ */