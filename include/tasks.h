#ifndef __TASKS_H__
#define __TASKS_H__

#include <string.h>
#include <stdint.h>

#include "pca9548.h"
#include "s13683.h"
#include "client.h"

void pca9548_task();
void s13673_task(s13683_sensor * arg);
void http_task(void *pvParameters);

#endif /* __TASKS_H__ */