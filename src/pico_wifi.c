#include "../include/pico_wifi.h"
#include "../include/pico_credentials.h"

PICO_WIFI_STATUS wifi_init() {
    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return WIFI_STATUS_NOT_CONNECTED;
    }

    // Enable wifi station
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS)) {
        printf("failed to connect.\n");
        return WIFI_STATUS_NOT_CONNECTED;
    }
    else {
        printf("Connected.\n");
        // Read the ip address in a human readable way
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }

    return WIFI_STATUS_CONNECTED;
}

PICO_WIFI_STATUS wifi_check_connection() {
    if(cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_JOIN) {
        printf("Connection lost....\n");
        printf("Trying to reconnect...\n");

        if(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, WIFI_CONNECT_TIMEOUT_MS) == 0) {
            printf("Reconnected!\n");
            return WIFI_STATUS_RE_CONNECTED;
        }

        return WIFI_STATUS_NOT_CONNECTED;
    }
    return WIFI_STATUS_CONNECTED;
}