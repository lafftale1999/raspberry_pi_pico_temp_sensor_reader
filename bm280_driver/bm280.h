/**
 * Copyright (C) 2025 Carl Broman
 *
 * @file        bm280.h
 * @author      Carl Broman <carl.broman@yh.nackademin.se>
 * @brief       Complete driver for the BM280 sensor: temperature, pressure, and humidity.
 * @addtogroup  bm280_driver BM280 Driver
 * @{
 *
 * @details
 * This is a complete driver for the BM280 sensor using I2C communication.
 * It includes JSON support to export measurement data in a structured format.
 *
 * Example usage:
 * @code
 *   #include "bm280.h"
 * 
 *   bm280_init(bm280_handle_t *handle, 0x76, INTERVAL_1000MS);
 *   bm280_read_data(*handle);
 *   http_post(bm280_get_json(*handle));
 * @endcode
 */

 /**
  * @defgroup bm280_headerfile BM280 Headerfile
  * @ingroup bm280_driver
  * @brief Headerfile for the BM280 driver.
  * @{
  */
#ifndef BM280_DRIVER_H_
#define BM280_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "pico/stdio.h"
#include "pico/stdlib.h"

/* -------------------------- DEFINES -------------------------------------------*/

/**
 * @defgroup bm280_config Configurations
 * @ingroup bm280_driver
 * @brief Defines that can be changed depending on usage.
 * @{
 */
#define BM280_UNITS_ON_BUS          1       /**< Number of BM280 units connected */
#define BM280_ADDRESSES_ON_BUS      {0x76}  /**< I2C addresses of connected BM280 units */
#define BM280_ADDRESS_LENGTH        7       /**< BM280 I2C address length (7-bit) */
/** @} */

/* -------------------------- MACROS -------------------------------------------*/
/**
 * @defgroup bm280_debug Debug Macro
 * @ingroup bm280_driver
 * @brief Macros for logging messages
 * @{
 */
#define BM280_DEBUG 0
#if BM280_DEBUG
#define BM280_LOG(...) printf(__VA_ARGS__)
#else
#define BM280_LOG(...)
#endif
/**@} */

/* -------------------------- TYPEDEFS -------------------------------------------*/

/**
 * @defgroup bm280_typedefs Typedefinitions
 * @ingroup bm280_driver
 * @brief External handle and predefined interval settings.
 * @{
 */

typedef struct bm280_handle_internal *bm280_handle_t; /**< Opaque handle to a BM280 instance */

typedef enum {
    INTERVAL_0_5MS      = 0b000,
    INTERVAL_62_5MS     = 0b001,
    INTERVAL_125MS      = 0b010,
    INTERVAL_250MS      = 0b011,
    INTERVAL_500MS      = 0b100,
    INTERVAL_1000MS     = 0b101,
    INTERVAL_10MS       = 0b110,
    INTERVAL_20MS       = 0b111
} BM280_READING_INTERVALS_MS; /**< Predefined standby intervals used in BM280 configuration */
/** @} */

/* -------------------------- FUNCTIONS -------------------------------------------*/

/**
 * @defgroup bm280_functions Functions
 * @ingroup bm280_driver
 * @brief Public functions provided by the BM280 driver.
 * @{
 */

/**
 * @brief Initialize the BM280 sensor.
 * @param handle Pointer to store the handle.
 * @param device_address I2C address.
 * @param interval Measurement standby interval.
 * @return PICO_W_OK if successful, otherwise PICO_W_FAIL.
 */
int bm280_init(bm280_handle_t *handle, const uint8_t device_address, const BM280_READING_INTERVALS_MS interval);

/**
 * @brief Read temperature, pressure, and humidity from the BM280.
 * @param handle Handle to the BM280 instance.
 * @return PICO_W_OK if successful, otherwise PICO_W_FAIL.
 */
int bm280_read_data(const bm280_handle_t handle);

/**
 * @brief Get latest measurement as a JSON string.
 * @param handle Handle to the BM280 instance.
 * @return Pointer to JSON string, or NULL if not available.
 */
const char* BM280_get_json(const bm280_handle_t handle);

/** @} */ // End of bm280_functions

/**@} */
#ifdef __cplusplus
}
#endif

#endif  // BM280_DRIVER_H_
/** @} */ // End of bm280_driver
