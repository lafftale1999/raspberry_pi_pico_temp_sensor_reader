#ifndef BM280_DRIVER_H
#define BM280_DRIVER_H

#include "config.h"
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
#define BM280_ID_REG                        0xD0                // BM280 chip id is 0x60

// RESET
#define BM280_RESET_REGISTER                0xE0                // Register for reset.
#define BM280_WRITE_RESET_BITS              0xB6                // The device is reset using the complete power-on-reset procedure. Must be written to BM280_RESET_REGISTER

// HUMIDITY
#define BM280_CONTROL_HUMIDITY              0xF2                // Controls the oversampling of humidity data. 
#define BM280_SET_HUM_OSRS(osrs_h)          ((osrs_h) & 0x07)   // use together with BM280_OSRS_XXX            

/* TEMPERATURE & PRESSURE
    BM280_SET_TEMP_OSRS(BM280_OSRS_XN) to set temperature OSRS
    BM280_SET_PRESS_OSRS(BM280_OSRS_XN) to set pressure OSRS
    BM280_SET_MODE(BM280_MODE_XXX) to set reader mode.
*/
#define BM280_CONTROL_MEASURE               0xF4                        // register for setting the temp and press OSRS, and setting the reader mode
#define BM280_SET_TEMP_OSRS(osrs_t)         (((osrs_t) & 0x07) << 5)    // use together with BM280_OSRS_XXX
#define BM280_SET_PRESS_OSRS(osrs_p)        (((osrs_p) & 0x07) << 2)    // use together with BM280_OSRS_XXX

#define BM280_MODE_NORMAL                   0b11                // Argument for BM280_SET_MODE | perpetual cycling of measurements and inactive periods.
#define BM280_MODE_SLEEP                    0b00                // Argument for BM280_SET_MODE | no operation, all registers accessible, lowest power, selected after startup
#define BM280_MODE_FORCED                   0b01                // Argument for BM280_SET_MODE | perform one measurement, store results and return to sleep mode
#define BM280_SET_MODE(mode)                ((mode) & 0x03)     // Use BM280_MODE_XXX to set specified mode accepted by BM280

/* all data registers for reading values*/

#define BM280_HUMIDITY_REG_MSB              0xFD
#define BM280_HUMIDITY_REG_LSB              0xFE 

#define BM280_PRESS_REG_MSB                 0xF7
#define BM280_PRESS_REG_LSB                 0xF8
#define BM280_PRESS_REG_XLSB                0xF9

#define BM280_TEMP_REG_MSB                  0xFA
#define BM280_TEMP_REG_LSB                  0xFB
#define BM280_TEMP_REG_XLSB                 0xFC

#define BM280_READ_VALUES_REG_START         0xF7
#define BM280_READ_VALUES_REG_LEN           8

// CONFIGURATION FOR BM280
#define BM280_CONFIG_REGISTER               0xF5
#define BM280_SET_STANDBY_MS(ms)            (((ms) & 0x07) << 5)    // Use together with BM280_READ_INTERVALS_MS
#define BM280_SET_IIR_FILTER(fc)            (((fc) & 0x07) << 2)    // Use together with BM280_SET_FILTER_XXX

// JSON HEADLINE DEFINES
#define BM280_JSON_TEMP_KEY                 "temperature"
#define BM280_JSON_PRESS_KEY                "pressure"
#define BM280_JSON_HUM_KEY                  "humidity"
#define BM280_JSON_KEYS                     {BM280_JSON_TEMP_KEY, BM280_JSON_HUM_KEY, BM280_JSON_PRESS_KEY}
#define BM280_JSON_KEYS_LEN                 20
#define BM280_JSON_KEYS_SIZE                3
#define BM280_JSON_VALS_LEN                 10
#define BM280_JSON_STR_MAX_LEN              ((BM280_JSON_KEYS_LEN + BM280_JSON_VALS_LEN + 5) * BM280_JSON_KEYS_SIZE)
#define BM280_JSON_BUF_SIZE                 64

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