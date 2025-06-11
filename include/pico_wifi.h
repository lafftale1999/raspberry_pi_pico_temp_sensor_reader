#ifndef PICO_WIFI_H
#define PICO_WIFI_H

#include "pico/cyw43_arch.h"

#include "config.h"

#define WIFI_SSID "Cbphone"
#define WIFI_PASS "test1234"

PICO_W_RETURN_STATUS wifi_init();

#endif