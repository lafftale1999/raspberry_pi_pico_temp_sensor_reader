#include "pico_mqtt.h"
#include "pico_credentials.h"
#include "pico_log.h"
#include "../certs/ca_cert.h"
#include "../certs/client_cert.h"
#include "../certs/client_key.h"

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "string.h"

#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"
#include "pico/unique_id.h"
#include "lwip/dns.h"
#include "lwip/altcp_tls.h"

struct MQTT_CLIENT_DATA_T{
    mqtt_client_t* mqtt_client_inst;
    struct mqtt_connect_client_info_t mqtt_client_info;
    char data[MQTT_OUTPUT_RINGBUF_SIZE];
    char topic[MQTT_TOPIC_LEN];
    uint32_t len;
    ip_addr_t mqtt_server_address;
    bool connect_done;
    int subscribe_count;
    bool stop_client;
};

static void pub_request_cb(__unused void *arg, err_t err) {
    if (err != 0) {
        PICO_LOGE("pub_request_cb failed %d", (uint8_t)err);
    }
}

static void sub_request_cb(void *arg, err_t err) {
    MQTT_client_handle_t handle = (MQTT_client_handle_t)arg;
    if (err != 0) {
        panic("subscribe request failed %d", err);
    }
    handle->subscribe_count++;
}

static void unsub_request_cb(void *arg, err_t err) {
    MQTT_client_handle_t handle = (MQTT_client_handle_t)arg;
    if (err != 0) {
        panic("unsubscribe request failed %d", err);
    }
    handle->subscribe_count--;
    assert(state->subscribe_count >= 0);

    // Stop if requested
    if (handle->subscribe_count <= 0 && handle->stop_client) {
        mqtt_disconnect(handle->mqtt_client_inst);
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    MQTT_client_handle_t handle = (MQTT_client_handle_t)arg;

    PICO_LOGI("mqtt_connection_cb called! status: %d\n", status);
    if (status == MQTT_CONNECT_ACCEPTED) {
        PICO_LOGI("MQTT connected!\n");
        handle->connect_done = true;

        // indicate online
        if (handle->mqtt_client_info.will_topic) {
            mqtt_publish(handle->mqtt_client_inst, handle->mqtt_client_info.will_topic, "1", 1, MQTT_LWT_QOS, true, pub_request_cb, handle);
        }

    } else if (status == MQTT_CONNECT_DISCONNECTED) {
        PICO_LOGE("Not MQTT connected!\n");
        if (!handle->connect_done) {
            panic("Failed to connect to mqtt server");
        }
    }
    else {
        panic("Unexpected status");
    }
}

static void mqtt_incoming_data_cb(void *handle, const u8_t *data, u16_t len, u8_t flags) {
    MQTT_client_handle_t state = (MQTT_client_handle_t)handle;
#if MQTT_UNIQUE_TOPIC
    const char *basic_topic = state->topic + strlen(state->mqtt_client_info.client_id) + 1;
#else
    const char *basic_topic = state->topic;
#endif
    strncpy(state->data, (const char *)data, len);
    state->len = len;
    state->data[len] = '\0';

    PICO_LOGI("Topic: %s, Message: %s\n", state->topic, state->data);
}

static void mqtt_incoming_publish_cb(void *handle, const char *topic, u32_t tot_len) {
    MQTT_client_handle_t state = (MQTT_client_handle_t)handle;
    strncpy(state->topic, topic, sizeof(state->topic));
}

static void start_client(MQTT_client_handle_t handle) {
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    const int port = MQTT_TLS_PORT;
    PICO_LOGI("Using TLS\n");
#else
    const int port = MQTT_PORT;
    PICO_LOGI("Warning: Not using TLS\n");
#endif

    handle->mqtt_client_inst = mqtt_client_new();
    if (!handle->mqtt_client_inst) {
        panic("MQTT client instance creation error");
    }
    PICO_LOGI("IP address of this device %s\n", ipaddr_ntoa(&(netif_list->ip_addr)));
    PICO_LOGI("Connecting to mqtt server at %s\n", ipaddr_ntoa(&handle->mqtt_server_address));

    cyw43_arch_lwip_begin();
    if (mqtt_client_connect(handle->mqtt_client_inst, &handle->mqtt_server_address, port, mqtt_connection_cb, handle, &handle->mqtt_client_info) != ERR_OK) {
        panic("MQTT broker connection error");
    }
#if LWIP_ALTCP && LWIP_ALTCP_TLS
    // This is important for MBEDTLS_SSL_SERVER_NAME_INDICATION
    mbedtls_ssl_set_hostname(altcp_tls_context(handle->mqtt_client_inst->conn), MQTT_SERVER);
#endif
    mqtt_set_inpub_callback(handle->mqtt_client_inst, mqtt_incoming_publish_cb, mqtt_incoming_data_cb, handle);
    cyw43_arch_lwip_end();
}

// Call back with a DNS result
static void dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    MQTT_client_handle_t handle = (MQTT_client_handle_t)arg;
    if (ipaddr) {
        handle->mqtt_server_address = *ipaddr;
        start_client(handle);
    } else {
        panic("dns request failed");
    }
}

uint8_t MQTT_open(MQTT_client_handle_t *handle) {
    MQTT_client_handle_t temp_handle = malloc(sizeof(struct MQTT_CLIENT_DATA_T));

    if(temp_handle == NULL) {
        PICO_LOGE("Failed to allocate MQTT handle in memory\n");
        goto exit;
    }

    // Create a unique ID for the device
    char unique_id_buf[sizeof(DEVICE_MODEL)];
    pico_get_unique_board_id_string(unique_id_buf, sizeof(unique_id_buf));
    char device_id[sizeof(DEVICE_MODEL) + sizeof(unique_id_buf) - 1];
    memcpy(device_id, DEVICE_MODEL, sizeof(DEVICE_MODEL) - 1);
    memcpy(&device_id[sizeof(DEVICE_MODEL)], unique_id_buf, sizeof(unique_id_buf) - 1);
    device_id[sizeof(device_id) - 1] = '\0';
    PICO_LOGI("Device id finished: %s\n", device_id);

    temp_handle->mqtt_client_info.client_id = device_id;

    // Decide how long the TCP connection should be kept alive between intervals
    temp_handle->mqtt_client_info.keep_alive = MQTT_KEEP_ALIVE_S;

    // Assign username and password
    #if defined(MQTT_USERNAME) && defined(MQTT_PASSWORD)
        temp_handle->mqtt_client_info.client_user = MQTT_USERNAME;
        temp_handle->mqtt_client_info.client_pass = MQTT_PASSWORD;
    #else
        temp_handle->mqtt_client_info.client_user = NULL;
        temp_handle->mqtt_client_info.client_pass = NULL;
    #endif

    // Assign Last Will Testament values
    // This will be sent when connection is correctly disconnected from broker
    temp_handle->mqtt_client_info.will_topic = MQTT_LWT_TOPIC;
    temp_handle->mqtt_client_info.will_msg = MQTT_LWT_MSG;
    temp_handle->mqtt_client_info.will_qos = MQTT_LWT_QOS;
    temp_handle->mqtt_client_info.will_retain = MQTT_LWT_RETAIN_MSG;

    // Configure the client for tls
    #if LWIP_ALTCP && LWIP_ALTCP_TLS
    #ifdef MQTT_CERT_INC
        
        // Configure for MTLS
        temp_handle->mqtt_client_info.tls_config = altcp_tls_create_config_client_2wayauth(
            (const u8_t *)ca_cert, sizeof(ca_cert),
            (const u8_t *)client_key, sizeof(client_key),
            CLIENT_KEY_PASS, CLIENT_KEY_PASS_LEN,
            (const u8_t *)client_cert, sizeof(client_cert)
        );

        if (temp_handle->mqtt_client_info.tls_config == NULL) {
            PICO_LOGE("TLS config creation failed!\n");
        } else {
            PICO_LOGI("TLS config OK\n");
        }

    #if ALTCP_MBEDTLS_AUTHMODE != MBEDTLS_SSL_VERIFY_REQUIRED
        PICO_LOGW("Warning: tls without verification is insecure\n");
    #endif
    #else
        // Configure for TLS
        state->client_info.tls_config = altcp_tls_create_config_client(NULL, 0);
        PICO_LOGW("Warning: tls without a certificate is insecure\n");
    #endif
    #endif
    
    // Depending if we need to use DNS to find the servers IP-address
    // Else just use the MQTT_SERVER address
    int err;
    #if defined(MQTT_DNS_NAME)
    cyw43_arch_lwip_begin();
    err = dns_gethostbyname(MQTT_SERVER, &temp_handle->mqtt_server_address, dns_found, &temp_handle);
    cyw43_arch_lwip_end();

    if(err) {
        PICO_LOGE("Failed to assign a correct IP-address for broker\n");
        goto exit;
    }
    #else
    err = ipaddr_aton(MQTT_SERVER, &temp_handle->mqtt_server_address);
    if(!err) {
        PICO_LOGE("Failed to assign a correct IP-address for broker\n");
        goto exit;
    }
    #endif
    
    start_client(temp_handle);

    while (!temp_handle->connect_done || !mqtt_client_is_connected(temp_handle->mqtt_client_inst)) {
        cyw43_arch_poll();
        cyw43_arch_wait_for_work_until(make_timeout_time_ms(10000));
    }

    *handle = temp_handle;

    PICO_LOGI("MQTT successfully connected!");

    return 0;
exit:

    if(temp_handle != NULL) {
        free(temp_handle);
    }

    return 1;
}

uint8_t MQTT_publish(MQTT_client_handle_t handle, const char *topic, const char *payload) {
    err_t err = mqtt_publish(handle->mqtt_client_inst, topic, payload, strlen(payload), MQTT_PUB_QOS, MQTT_PUB_RETAIN, pub_request_cb, handle);

    if(err == ERR_OK) return 0;
    else if(err == ERR_MEM) {
        PICO_LOGE("Out of memory error\n");
        return 1;
    }
    else if(err == ERR_CONN) {
        PICO_LOGE("Not connected to broker\n");
        return 1;
    }
}

uint8_t MQTT_subscribe(MQTT_client_handle_t handle, const char *topic) {
    if(mqtt_sub_unsub(handle->mqtt_client_inst, topic, MQTT_SUB_QOS, pub_request_cb, handle, true) == 0) {
        return 0;
    }
    return 1;
}

uint8_t MQTT_unsubscribe(MQTT_client_handle_t handle, const char *topic) { 
    if(mqtt_sub_unsub(handle->mqtt_client_inst, topic, MQTT_SUB_QOS, unsub_request_cb, handle, false) == 0) {
        return 0;
    }
    return 1;
}

void MQTT_close(MQTT_client_handle_t handle) {
    mqtt_disconnect(handle->mqtt_client_inst);
    free(handle);
}

uint8_t MQTT_poll(MQTT_client_handle_t handle) {
    if (!handle) return 1;
    
    if (!mqtt_client_is_connected(handle->mqtt_client_inst)) {
        PICO_LOGE("MQTT client not connected\n");
        return 1;
    }

    return 0;
}

uint8_t MQTT_reconnect(MQTT_client_handle_t *handle) {
    MQTT_close(*handle);
    return MQTT_open(handle);
}