#ifndef RPI_PIC_W_I2C_H
#define RPI_PIC_W_I2C_H

#include <stdint.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#define I2C_SDA_PIN                 14              // GPIO
#define I2C_SCL_PIN                 15              // GPIO 
#define I2C_PORT                    i2c1            // Which I2C module is used for communication
#define I2C_FREQUENCY               100000          // Frequency for communication

/**
 * @brief Initializes the i2c communication.
 * 
 * @return 0 for success.
 */
uint8_t i2c_open();

/**
 * @brief Writes data on the initialized databus.
 * 
 * @param[in] device_address address for targeted device on the i2c bus.
 * @param[in] data pointer to array of bytes
 * @param[in] len length of array
 * @param[in] keepMaster true = keeps the master over bus | false = lets go of control over bus after transmitting data
 * 
 * @return 0 for success. 1 for fail.
 */
uint8_t i2c_write_data(const uint8_t *device_address, uint8_t *data, const size_t len, bool keepMaster);

/**
 * @brief Reads data from assigned address and increments the register address by 1 for _len long.
 * 
 * @param[in] device_address address for targeted device on the i2c bus.
 * @param[in] d_register pointer to variable containing the register address
 * @param[in] buf pointer to the byte array to write data to
 * @param[in] len the size of buf
 * 
 * @return 0 for success. 1 for fail.
 */
uint8_t i2c_read_data(const uint8_t *device_address, const uint8_t *d_register, uint8_t *buf, const size_t len);

/**
 * @brief Debug function for finding devices on the i2c bus.
 */
uint8_t i2c_scan_bus();

#endif