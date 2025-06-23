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

/**
 * @brief initializes the MQTT protocol. Points the 
 * @param[out] handle Opaque pointer to the internal datastructure for the MQTT protocol. Succesful initialization re-directs the pointer to the datastructure on heap.
 * 
 * @return 0 for succesful init. 1 for failed init.
 */
uint8_t MQTT_open(MQTT_client_handle_t *handle);

/**
 * @brief publishes data to broker.
 * 
 * @param[in] handle Opaque pointer to internal MQTT handle datastructure
 * @param[in] topic The topic the payload will be sent in
 * @param[in] payload The payload to be published
 * 
 * @return 0 for success. 1 for failed.
 */
uint8_t MQTT_publish(MQTT_client_handle_t handle, const char *topic, const char *payload);

/**
 * @brief subscribes to passed topic.
 * 
 * @param[in] handle Opaque pointer to internal MQTT handle datastructure
 * @param[in] topic The topic to subscribe to
 * 
 * @return 0 for success. 1 for failed.
 */
uint8_t MQTT_subscribe(MQTT_client_handle_t handle, const char *topic);

/**
 * @brief unsubscribes from the passed topic.
 * 
 * @param[in] handle Opaque pointer to internal MQTT handle datastructure
 * @param[in] topic The topic to unsubscribe from
 * 
 * @return 0 for succcess. 1 for failed.
 */
uint8_t MQTT_unsubscribe(MQTT_client_handle_t handle, const char *topic);

/**
 * @brief closes the connection and unitiliazes the handle and free's the memory on the heap.
 * 
 * @param[out] handle Opaque pointer to internal MQTT handle datastructure
 */
void MQTT_close(MQTT_client_handle_t handle);

/**
 * @brief checks that the handle is still pointing to a place in memory and that the
 * client is still connected to the broker.
 * 
 * @param handle Opaque pointer to internal MQTT handle datastructure
 * 
 * @return 0 for connected. 1 for not connected.
 */
uint8_t MQTT_poll(MQTT_client_handle_t handle);

/**
 * @brief Attempts to reconnect to the broker. De- and Re-initializes the handle.
 * 
 * @param[in] handle Opaque pointer to internal MQTT handle datastructure
 * 
 * @return 0 for success. 1 for failed.
 */
uint8_t MQTT_reconnect(MQTT_client_handle_t *handle);

#endif