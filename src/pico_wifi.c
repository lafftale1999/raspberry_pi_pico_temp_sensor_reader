#include "../include/pico_wifi.h"
#include "pico_log.h"
#include "../include/pico_credentials.h"

PICO_WIFI_STATUS wifi_init() {
    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        PICO_LOGE("Wi-Fi init failed\n");
        return WIFI_STATUS_NOT_CONNECTED;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    PICO_LOGI("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS)) {
        PICO_LOGE("failed to connect.\n");
        return WIFI_STATUS_NOT_CONNECTED;
    }
    else {
        PICO_LOGI("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        PICO_LOGI("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    return WIFI_STATUS_CONNECTED;
}

PICO_WIFI_STATUS wifi_check_connection() {
    if(cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_JOIN) {
        PICO_LOGE("Connection lost....\n");
        PICO_LOGE("Trying to reconnect...\n");

        if(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS) == 0) {
            PICO_LOGI("Reconnected!\n");
            return WIFI_STATUS_RE_CONNECTED;
        }

        return WIFI_STATUS_NOT_CONNECTED;
    }
    return WIFI_STATUS_CONNECTED;
}