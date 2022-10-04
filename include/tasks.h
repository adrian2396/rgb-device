#ifndef __TASKS_H__
#define __TASKS_H__

#include <string.h>
#include <stdint.h>

#include "pca9548.h"
#include "s13683.h"
#include "tps61165.h"
#include "client.h"

TaskFunction_t  pca9548_task(pca9548_switch *pca);
TaskFunction_t  s13673_task(s13683_sensor * arg);
TaskFunction_t  tps61165_task(tps61165_driver *driver);
TaskFunction_t  http_task(void *pvParameters);

#endif /* __TASKS_H__ */