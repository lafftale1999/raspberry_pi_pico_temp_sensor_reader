#ifndef PICO_WIFI_H
#define PICO_WIFI_H

#include "pico/cyw43_arch.h"

#define WIFI_CONNECT_TIMEOUT_MS 20000

/**
 * @brief enum for wifi statuses as return values
 */
typedef enum{
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_NOT_CONNECTED,
    WIFI_STATUS_RE_CONNECTED
}PICO_WIFI_STATUS;

/**
 * @brief Initialize the Wifi with the lwIP stack with the cyw43 chip.
 * 
 * @return WIFI_STATUS_CONNECTED for succesfull init. WIFI_STATUS_NOT_CONNECTED for failed init.
 */
PICO_WIFI_STATUS wifi_init();

/**
 * @brief Checks the connection of the device. 
 * 
 * @return WIFI_STATUS_CONNECTED for stable connection | WIFI_STATUS_RE_CONNECTED if re-connected | WIFI_STATUS_NOT_CONNECTED if unable to re-connect.
 */
PICO_WIFI_STATUS wifi_check_connection();

#endif