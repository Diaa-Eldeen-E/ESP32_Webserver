#ifndef WIFI_H
#define WIFI_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "esp_wifi.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "freertos/event_groups.h"

#define EXAMPLE_ESP_WIFI_SSID "diaa"
#define EXAMPLE_ESP_WIFI_PASS "x12345678"
#define EXAMPLE_ESP_MAXIMUM_RETRY 8

void wifi_init_sta(void);

#endif /* WIFI_H */
