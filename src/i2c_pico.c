#include "pico/stdio.h"
#include "pico_log.h"
#include <stdio.h>
#include "include/i2c_pico.h"

uint8_t i2c_open() {
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    return 0;
}

uint8_t i2c_write_data(const uint8_t *device_address, uint8_t *data, const size_t len, bool keepMaster) {
    int err = i2c_write_blocking(I2C_PORT, *device_address, data, len, keepMaster);

    if(err == PICO_ERROR_GENERIC) {
        return 1;
    }

    return 0;
}


uint8_t i2c_read_data(const uint8_t *device_address, const uint8_t *d_register, uint8_t *buf, const size_t buf_size) {
    // Start with a pointer to where we should start reading.
    int err = i2c_write_blocking(I2C_PORT, *device_address, d_register, 1, true);
    if (err == PICO_ERROR_GENERIC) {
        PICO_LOGE("i2c_read_data() | Unable to write to i2c buss");
        return 1;
    }

    // The BM280s data registers are autoincrementing - so start reading from a certain address ex 0xF7 (pressure MSB) -> 0xFE (humidity LSB).
    err = i2c_read_blocking(I2C_PORT, *device_address, buf, buf_size, false);
    if (err == PICO_ERROR_GENERIC) {
        PICO_LOGE("i2c_read_data() | Unable to read from i2c buss");
        return 1;
    }

    return 0;
}

uint8_t i2c_scan_bus() {
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (addr < 0x08 || addr > 0x77) {
            ret = PICO_ERROR_GENERIC;
        } else {
            absolute_time_t timeout = make_timeout_time_ms(10);
            ret = i2c_read_blocking_until(I2C_PORT, addr, &rxdata, 1, false, timeout);
        }

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
        sleep_ms(100);
    }
    printf("Done.\n");

    return 0;
}