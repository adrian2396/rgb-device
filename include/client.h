#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_http_client.h"

esp_err_t _http_event_handler(esp_http_client_event_t *evt);

void http_rest_with_url(void);

void https_async(void);

#endif /* __CLIENT_H__ */