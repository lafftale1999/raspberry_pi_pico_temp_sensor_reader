#ifndef PICO_WIFI_H
#define PICO_WIFI_H

#include "pico/cyw43_arch.h"

#include "config.h"

#define WIFI_CONNECT_TIMEOUT_MS 20000

typedef enum{
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_NOT_CONNECTED,
    WIFI_STATUS_RE_CONNECTED
}PICO_WIFI_STATUS;

PICO_WIFI_STATUS wifi_init();
PICO_WIFI_STATUS wifi_check_connection();

#endif