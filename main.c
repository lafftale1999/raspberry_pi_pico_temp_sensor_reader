    #include "include/i2c_pico.h"
    #include "pico_log.h"
    #include "bm280_driver/bm280.h"
    #include "include/pico_wifi.h"
    #include "include/pico_mqtt.h"
    #include "pico/time.h"
    #include "hardware/watchdog.h"

    #define WIFI_POLLING_MS 100
    #define DEVICE_POLLING_MS 5000
    #define MQTT_PUBLISH_MS 20000
    #define BLINK_INTERVAL_MS 1000

    int main()
    {
        stdio_init_all();
        sleep_ms(5000);

        i2c_open();

        bm280_handle_t bm280_handle = NULL;
        if (bm280_init(&bm280_handle, 0x76, INTERVAL_1000MS) == 0) {
            PICO_LOGI("device succesfully init\n");
        }

        wifi_init();

        MQTT_client_handle_t mqtt_handle = NULL;
        if (MQTT_open(&mqtt_handle) == 0) {
            PICO_LOGI("Mqtt initiated correctly\n");
        }
        
        absolute_time_t next_device_poll = make_timeout_time_ms(DEVICE_POLLING_MS);
        absolute_time_t next_publish = make_timeout_time_ms(MQTT_PUBLISH_MS);
        absolute_time_t next_blink = make_timeout_time_ms(BLINK_INTERVAL_MS);

        uint8_t led_on = 0;

        int err = 0;

        while(1) {
            cyw43_arch_poll();

            err = wifi_check_connection();

            if(err == WIFI_STATUS_CONNECTED) {
                if (MQTT_poll(mqtt_handle) != 0) {
                    if(MQTT_reconnect(&mqtt_handle) != 0) {
                        watchdog_reboot(0,0,0);
                    }
                }

                if(absolute_time_diff_us(get_absolute_time(), next_publish) <= 0) {
                    PICO_LOGI("Publish to topic\n");

                    if (MQTT_publish(mqtt_handle, "/room_meas", BM280_get_json(bm280_handle)) != 0) {
                        PICO_LOGI("Publish failed\n");
                    }

                    next_publish = make_timeout_time_ms(MQTT_PUBLISH_MS);
                }

                if(absolute_time_diff_us(get_absolute_time(), next_device_poll) <= 0) {
                    if (bm280_read_data(bm280_handle) != 0) {
                        PICO_LOGI("Failed to read data from BM280\n");
                    }

                    next_device_poll = make_timeout_time_ms(DEVICE_POLLING_MS);
                }

                if(absolute_time_diff_us(get_absolute_time(), next_blink) <= 0) {
                    led_on = !led_on;
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);

                    next_blink = make_timeout_time_ms(BLINK_INTERVAL_MS);
                }
            }

            else if(err == WIFI_STATUS_RE_CONNECTED) {
                if(MQTT_reconnect(&mqtt_handle) != 0) {
                    watchdog_reboot(0,0,0);
                }
            }

            else if(err == WIFI_STATUS_NOT_CONNECTED) {
                watchdog_reboot(0,0,0);
            }
            
            cyw43_arch_wait_for_work_until(make_timeout_time_ms(100));
        }
    }