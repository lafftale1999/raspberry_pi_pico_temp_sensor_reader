#ifndef PICO_W_MQTT_H
#define PICO_W_MQTT_H

#include <stdint.h>

#define MQTT_SERVER         "192.168.61.111"

#define MQTT_TOPIC_LEN      100
#define MQTT_KEEP_ALIVE_S   60

#define CLIENT_KEY_PASS     NULL
#define CLIENT_KEY_PASS_LEN 0

#define MQTT_TLS_PORT       8883
#define MQTT_PORT           8883

// PUBLISH SETTINGS

#define MQTT_PUB_QOS        1
#define MQTT_PUB_RETAIN     true

// SUBSCRIBE SETTINGS

#define MQTT_SUB_QOS        1

// LWT SETTINGS

#define MQTT_LWT_QOS        1
#define MQTT_LWT_MSG        "0"
#define MQTT_LWT_TOPIC      "/online"
#define MQTT_LWT_RETAIN_MSG true


typedef struct MQTT_CLIENT_DATA_T *MQTT_client_handle_t;

uint8_t MQTT_open(MQTT_client_handle_t *handle);
uint8_t MQTT_publish(MQTT_client_handle_t handle, const char *topic, const char *payload);
uint8_t MQTT_subscribe(MQTT_client_handle_t handle, const char *topic);
uint8_t MQTT_unsubscribe(MQTT_client_handle_t handle, const char *topic);
void MQTT_close(MQTT_client_handle_t handle);
uint8_t MQTT_poll(MQTT_client_handle_t handle);
uint8_t MQTT_reconnect(MQTT_client_handle_t *handle);

#endif