#ifndef RPI_PIC_W_I2C_H
#define RPI_PIC_W_I2C_H

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "config.h"

#define I2C_SDA_PIN                 14              // GPIO
#define I2C_SCL_PIN                 15              // GPIO 
#define I2C_PORT                    i2c1            // Which I2C module is used for communication
#define I2C_FREQUENCY               100000          // Frequency for communication

PICO_W_RETURN_STATUS _i2c_init();
PICO_W_RETURN_STATUS write_data(const uint8_t *device_address, uint8_t *data, const size_t len, bool keepMaster);
PICO_W_RETURN_STATUS read_data(const uint8_t *device_address, const uint8_t *d_register, uint8_t *buf, const size_t len);
PICO_W_RETURN_STATUS scan_i2c_bus();

#endif