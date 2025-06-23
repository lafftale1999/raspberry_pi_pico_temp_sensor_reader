#ifndef RPI_PIC_W_I2C_H
#define RPI_PIC_W_I2C_H

#include <stdint.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define I2C_SDA_PIN                 14              // GPIO
#define I2C_SCL_PIN                 15              // GPIO 
#define I2C_PORT                    i2c1            // Which I2C module is used for communication
#define I2C_FREQUENCY               100000          // Frequency for communication

uint8_t i2c_open();
uint8_t i2c_write_data(const uint8_t *device_address, uint8_t *data, const size_t len, bool keepMaster);
uint8_t i2c_read_data(const uint8_t *device_address, const uint8_t *d_register, uint8_t *buf, const size_t len);
uint8_t scan_i2c_bus();

#endif