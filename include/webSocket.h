#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include <stdio.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"

#define NO_DATA_TIMEOUT_SEC 10

static void shutdown_signaler(TimerHandle_t xTimer);
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void websocket_app_start(void);

#endif /* __WEBSOCKET_H__ */