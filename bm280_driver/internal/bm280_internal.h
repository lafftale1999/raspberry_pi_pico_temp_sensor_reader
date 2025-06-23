/**
 * Copyright (C) 2025 Carl Broman
 *
 * @file        bm280_internal.h
 * @author      Carl Broman <carl.broman@yh.nackademin.se>
 * @brief       Internal API for the BM280 sensor: JSON parser, Calibrating and Converting measurements
 * @defgroup  bm280_driver_internal BM280 Driver Internal
 * @{
 *
 * @details
 * This header is for internal use by the BM280 driver implementation. Do not include this file outside bm280.c.
 */

#ifndef BM280_DRIVER_INTERNAL_H_
#define BM280_DRIVER_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

#include "bm280_json.h"
#include "bm280_settings.h"
#include "bm280.h"

/**
 * @brief Internal struct for BM280 driver state and calibration.
 */
struct bm280_handle_internal{
    int32_t t_fine;         /**< Used internally for compensation calculations. */
    int32_t temperature;    /**< Compensated temperature in hundredths of degrees Celsius. */
    uint32_t pressure;      /**< Compensated pressure in Pascal. */
    uint32_t humidity;      /**< Compensated humidity in 1/1024 %RH. */

    int32_t temp_raw;       /**< Raw temperature reading from sensor. */
    int32_t press_raw;      /**< Raw pressure reading from sensor. */
    int32_t hum_raw;        /**< Raw humidity reading from sensor. */

    uint8_t i2c_address;    /**< I2C address of the sensor. */
    uint8_t address_length; /**< I2C address length. */

    char json_string[BM280_JSON_STR_MAX_LEN]; /**< JSON string buffer. */
    bool json_parsed; /**< Flag if JSON is parsed. */

    /**
     * @brief Calibration values grouped by sensor type.
     */
    struct {
        struct {
            uint16_t dig_t1;
            int16_t dig_t2;
            int16_t dig_t3;
        } temp;

        struct {
            uint16_t dig_p1;
            int16_t dig_p2;
            int16_t dig_p3;
            int16_t dig_p4;
            int16_t dig_p5;
            int16_t dig_p6;
            int16_t dig_p7;
            int16_t dig_p8;
            int16_t dig_p9;
        } press;

        struct {
            uint8_t dig_h1;
            int16_t dig_h2;
            uint8_t dig_h3;
            int16_t dig_h4;
            int16_t dig_h5;
            int8_t dig_h6;
        } hum;
    } cal_val;
};

/**
 * @defgroup BM280_internal_functions Internal functions
 * @ingroup bm280_driver_internal
 * @brief All internal functions abstracted from the external interface.
 * @{
 */
    /**
     * @brief Parses the current sensor readings into a JSON-formatted string.
     *
     * @param handle Handle to the BM280 device.
     * @return Always returns 0. Updates internal `json_string` in handle.
     */
    int bm280_parse_json(const bm280_handle_t handle);

    /**
     * @brief Reads and sets calibration parameters from the sensor.
     *
     * @param handle BM280 device handle.
     * @return 0 on success, 1 on failure.
     */
    int bm280_calibration(const bm280_handle_t handle);

    /**
     * @brief Converts the raw pressure value to readable format using calibration parameters.
     * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
     * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
     * 
     * @param handle BM280 device handle.
     * @return 0 on success, 1 if division by zero occurs.
     */
    int bm280_convert_pressure(const bm280_handle_t handle);

    /**
     * @brief Converts the raw temperature value to readable format and calculates t_fine.
     * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
     * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf 
     * 
     * @param handle BM280 device handle.
     * @return 0 on success.
     */
    int bm280_convert_temperature(const bm280_handle_t handle);

    /**
     * @brief Converts the raw humidity value to readable format using calibration data.
     * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
     * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf 
     *
     * @param handle BM280 device handle.
     * @return 0 on success.
     */
    int bm280_convert_humidity(const bm280_handle_t handle);


    /**
     * @brief Converts all raw sensor values (temperature, pressure, humidity).
     *
     * @param handle BM280 device handle.
     * @return 0 if all conversions succeed, otherwise 1.
     */
    int bm280_convert_measurements(const bm280_handle_t handle);
/**@} */

#ifdef __cplusplus
}
#endif

#endif

/**@} */