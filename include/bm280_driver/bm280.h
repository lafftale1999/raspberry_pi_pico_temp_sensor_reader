/**
 * Copyright (C) 2025 Carl Broman
 *
 * @file:       bm280_driver.h
 * @author:     Carl Broman <carl.broman@yh.nackademin.se>
 * @brief:  The complete driver for the BM280 sensor. Measuring temperature, pressure and humidity.
 * @addtogroup bm280_driver BM280_driver
 * @{
 * @brief Driver for the bm280 sensor.
 * @details This is a complete driver for the bm280 sensor based on the i2c communication protocol. 
 * The driver has a JSON-parser incorporated to be able to send its sensor-data in a unified way.
 *
 * Example usage:
 * @code
 *   bm280_init(bm280_handle_t *handle, const uint8_t device_address, const BM280_READING_INTERVALS_MS interval);
 *   bm280_read_data(bm280_handle_t *handle);
 * @endcode
 -------------------------------------------------------------------------------------------------*/

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

#include "pico_error.h"
#include "bm280_config.h"

/* Exported defines ------------------------------------------------------------------------------*/
/** @addtogroup template_exported_defines Exported defines
 *  @{ */
/** @} template_exported_defines */

/* Exported macros -------------------------------------------------------------------------------*/
/** @addtogroup template_exported_macros Exported macros
 *  @{ */
/** @} template_exported_macros */

/* Exported type definitions ---------------------------------------------------------------------*/
/** @addtogroup template_exported_typedefs Exported type definitions
 *  @{ */
/** @} template_exported_typedefs */

/* Exported function prototypes ------------------------------------------------------------------*/
/** @addtogroup template_exported_functions Exported functions
 *  @{ */
void template_init(void);
/** @} template_exported_functions */

#ifdef __cplusplus
}
#endif

#endif  // SRC_TEMPLATE_TEMPLATE_H_
/** @} template */



// ------------------------------------------------


#ifndef BM280_DRIVER_H
#define BM280_DRIVER_H

#include "pico_error.h"
#include <stdlib.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdio.h"

// *********************************
// ******** CONFIGURATIONS *********
// *********************************
#define BM280_UNITS_ON_BUS                  1           // The amount of BM280 units on the I2C bus.
#define BM280_ADDRESSES_ON_BUS              {0x76}      // Array of addresses for BM280 units on the I2C bus.
#define BM280_ADDRESS_LENGTH                7           // Length of address in bits.


// *********************************
// ********* CALIBRATIONS **********
// *********************************

// Measurements parameters
// Temperature
#define BM280_TEMP_CALIB_PARAM_START        0x88
#define BM280_TEMP_PARAMS_LEN               6

// Pressure
#define BM280_PRESS_CALIB_PARAM_START       0x8E
#define BM280_PRESS_PARAMS_LEN              18

// Humidity
#define BM280_HUM_CALIB_FIRST               0xA1
#define BM280_HUM_CALIB_FIRST_LEN           1
#define BM280_HUM_CALIB_CONTINUE            0xE1
#define BM280_HUM_CALIB_CONTINUE_LEN        8

#define BM280_AMOUNT_CALIB_PARAMS           (BM280_TEMP_PARAMS_LEN + BM280_PRESS_PARAMS_LEN + BM280_HUM_CALIB_FIRST_LEN + BM280_HUM_CALIB_CONTINUE_LEN)

// OVERSAMPLING RATES
#define BM280_OSRS_SKIPPED                  0b000
#define BM280_OSRS_X1                       0b001
#define BM280_OSRS_X2                       0b010
#define BM280_OSRS_X4                       0b011
#define BM280_OSRS_X8                       0b100
#define BM280_OSRS_X16                      0b101    

//FILTER SETTINGS
#define BM280_SET_FILTER_OFF                0b000
#define BM280_SET_FILTER_2                  0b001
#define BM280_SET_FILTER_4                  0b010
#define BM280_SET_FILTER_8                  0b011
#define BM280_SET_FILTER_16                 0b100

// *********************************
// *********** REGISTERS ***********
// *********************************
// READ ID
// CHECK

// RESET
// CHECK

// HUMIDITY
         

/* TEMPERATURE & PRESSURE
    BM280_SET_TEMP_OSRS(BM280_OSRS_XN) to set temperature OSRS
    BM280_SET_PRESS_OSRS(BM280_OSRS_XN) to set pressure OSRS
    BM280_SET_MODE(BM280_MODE_XXX) to set reader mode.
*/
//CHECKK

/* all data registers for reading values*/
// CHECK

// CONFIGURATION FOR BM280


// JSON HEADLINE DEFINES
// CHECKED

// Handle for bm280 driver
typedef struct bm280_handle_internal *bm280_handle_t;

// bits to set the predetermined intervals for the BM280
typedef enum {
    INTERVAL_0_5MS      = 0b000,
    INTERVAL_62_5MS     = 0b001,
    INTERVAL_125MS      = 0b010,
    INTEVAL_250MS       = 0b011,
    INTERVAL_500MS      = 0b100,
    INTERVAL_1000MS     = 0b101,
    INTERVAL_10MS       = 0b110,
    INTERVAL_20MS       = 0b111
} BM280_READING_INTERVALS_MS;

/*!
    Initializes the BM280 by allocating the datastructure on the heap, fills the calibration parameters and configure its set-up.

    \param handle pointer to the handle structure.
    \param device_address address to the device on the I2C bus.
    \param interval enum of predetermined intervals defined for the BM280.

    \return PICO_W_OK successfully initialized. PICO_W_FAIL unable to initialize.
*/
PICO_W_RETURN_STATUS bm280_init(bm280_handle_t *handle, const uint8_t device_address, const BM280_READING_INTERVALS_MS interval);

/*!
    Reads the data from the device related to the handle passed in argument.

    \param handle device handle for reading data

    \return PICO_W_OK data has been read and saved properly. PICO_W_FAIL data was unable to be read.
*/
PICO_W_RETURN_STATUS bm280_read_data(bm280_handle_t handle);

const char* BM280_get_json(bm280_handle_t handle);

#endif