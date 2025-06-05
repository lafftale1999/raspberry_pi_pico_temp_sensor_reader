#ifndef PICO_WIFI_H
#define PICO_WIFI_H

#include "pico/cyw43_arch.h"

#include "config.h"

#define WIFI_SSID "YourWifi"
#define WIFI_PASS "YourPassword"

PICO_W_RETURN_STATUS wifi_init();

#endif