    #include "include/i2c_pico.h"
    #include "bm280_driver/bm280.h"
    #include "include/pico_wifi.h"
    #include "include/pico_mqtt.h"
    #include "pico/time.h"

    #define WIFI_POLLING_MS 100
    #define DEVICE_POLLING_MS 5000
    #define MQTT_PUBLISH_MS 30000

    int main()
    {
        stdio_init_all();
        sleep_ms(5000);

        printf("Start I2C init\n");
        if (i2c_open() == PICO_W_OK) {
            printf("i2c init succesful!\n");
        }
        
        bm280_handle_t bm280_handle = NULL;
        if (bm280_init(&bm280_handle, 0x76, INTERVAL_1000MS) == PICO_W_OK) {
            printf("device succesfully init\n");
        }

        wifi_init();

        MQTT_client_handle_t mqtt_handle = NULL;
        if (MQTT_open(&mqtt_handle) == 0) {
            printf("Mqtt initiated correctly\n");
        }
        
        absolute_time_t next_device_poll = make_timeout_time_ms(DEVICE_POLLING_MS);
        absolute_time_t next_publish = make_timeout_time_ms(MQTT_PUBLISH_MS);

        while(1) {
            cyw43_arch_poll();

            if (MQTT_poll(mqtt_handle) != 0) {
                MQTT_LOG("Unable to send messages\n");
            }

            if(absolute_time_diff_us(get_absolute_time(), next_publish) <= 0) {
                MQTT_LOG("Publish to topic\n");

                if (MQTT_publish(mqtt_handle, "/room_meas", BM280_get_json(bm280_handle)) != 0) {
                    MQTT_LOG("Publish failed\n");
                }

                next_publish = make_timeout_time_ms(MQTT_PUBLISH_MS);
            }

            if(absolute_time_diff_us(get_absolute_time(), next_device_poll) <= 0) {
                if (bm280_read_data(bm280_handle) != PICO_W_OK) {
                    BM280_LOG("Failed to read data from BM280\n");
                }

                next_device_poll = make_timeout_time_ms(DEVICE_POLLING_MS);
            }

            cyw43_arch_wait_for_work_until(make_timeout_time_ms(WIFI_POLLING_MS));
        }
    }