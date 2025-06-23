#include "bm280_internal.h"
#include "../include/i2c_pico.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"

/**
 * @brief Parses the current sensor readings into a JSON-formatted string.
 *
 * @param handle Handle to the BM280 device.
 * @return Always returns 0. Updates internal `json_string` in handle.
 */
int bm280_parse_json(const bm280_handle_t handle)
{
    handle->json_parsed = true;
    const char* keys[BM280_JSON_KEYS_LEN] = BM280_JSON_KEYS;
    char buf[BM280_JSON_BUF_SIZE];
    
    snprintf(handle->json_string, BM280_JSON_STR_MAX_LEN, "{");

    for(size_t i = 0; i < BM280_JSON_KEYS_SIZE; i++) {
        if (strcmp(keys[i], BM280_JSON_TEMP_KEY) == 0) {
            snprintf(buf, sizeof(buf), "\"%s\":%u", keys[i], handle->temperature);
        }
        else if(strcmp(keys[i], BM280_JSON_PRESS_KEY) == 0) {
            snprintf(buf, sizeof(buf), "\"%s\":%d", keys[i], handle->pressure);
        }
        else if(strcmp(keys[i], BM280_JSON_HUM_KEY) == 0) {
            snprintf(buf, sizeof(buf), "\"%s\":%d", keys[i], handle->humidity);
        }


        strncat(handle->json_string, buf, BM280_JSON_STR_MAX_LEN - strlen(handle->json_string) - 1);
        if (i < BM280_JSON_KEYS_SIZE - 1) {
            strncat(handle->json_string, ",", BM280_JSON_STR_MAX_LEN - strlen(handle->json_string) - 1);
        }
    }

    strncat(handle->json_string, "}", BM280_JSON_STR_MAX_LEN - strlen(handle->json_string) - 1);

    return 0;
}

/**
 * @brief Reads and sets calibration parameters from the sensor.
 *
 * @param handle BM280 device handle.
 * @return 0 on success, 1 on failure.
 */
int bm280_calibration(const bm280_handle_t handle)
{
    
    uint8_t buf[BM280_AMOUNT_CALIB_PARAMS] = {0};
    
    uint8_t t_p_param_start = BM280_TEMP_CALIB_PARAM_START;
    // read temperature and pressure calibration values
    
    if (i2c_read_data(&handle->i2c_address, &t_p_param_start, buf, BM280_TEMP_PARAMS_LEN + BM280_PRESS_PARAMS_LEN) != 0) {
        BM280_LOG("Unable to read temperature and pressure calibration values\n");
        return 1;
    }
    
    uint8_t hum_param[2] = {
        BM280_HUM_CALIB_FIRST,
        BM280_HUM_CALIB_CONTINUE
    };

    // read humidity calibration values
    if (i2c_read_data(&handle->i2c_address, &hum_param[0], &buf[24], BM280_HUM_CALIB_FIRST_LEN) != 0 ||
        i2c_read_data(&handle->i2c_address, &hum_param[1], &buf[25], BM280_HUM_CALIB_CONTINUE_LEN) != 0) {
        BM280_LOG("Unable to read humidity calibration values\n");
        return 1;
    }

    // save values
    // temperatures
    handle->cal_val.temp.dig_t1 = (uint16_t)(buf[1] << 8) | buf[0];
    handle->cal_val.temp.dig_t2 = (int16_t)(buf[3] << 8) | buf[2];
    handle->cal_val.temp.dig_t3 = (int16_t)(buf[5] << 8) | buf[4];

    // pressure
    handle->cal_val.press.dig_p1 = (uint16_t)(buf[7] << 8) | buf[6];
    handle->cal_val.press.dig_p2 = (int16_t)(buf[9] << 8) | buf[8];
    handle->cal_val.press.dig_p3 = (int16_t)(buf[11] << 8) | buf[10];
    handle->cal_val.press.dig_p4 = (int16_t)(buf[13] << 8) | buf[12];
    handle->cal_val.press.dig_p5 = (int16_t)(buf[15] << 8) | buf[14];
    handle->cal_val.press.dig_p6 = (int16_t)(buf[17] << 8) | buf[16];
    handle->cal_val.press.dig_p7 = (int16_t)(buf[19] << 8) | buf[18];
    handle->cal_val.press.dig_p8 = (int16_t)(buf[21] << 8) | buf[20];
    handle->cal_val.press.dig_p9 = (int16_t)(buf[23] << 8) | buf[22];
    
    // humidity
    handle->cal_val.hum.dig_h1 = buf[24];
    handle->cal_val.hum.dig_h2 = (int16_t)(buf[26] << 8) | buf[25];
    handle->cal_val.hum.dig_h3 = buf[27];
    handle->cal_val.hum.dig_h4 = (int16_t)((buf[28] << 4) | (buf[29] & 0x0F));
    handle->cal_val.hum.dig_h5 = (int16_t)((buf[30] << 4) | (buf[29] >> 4));
    handle->cal_val.hum.dig_h6 = buf[32];

    return 0;
}

/**
 * @brief Converts the raw pressure value to readable format using calibration parameters.
 * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
 * 
 * @param handle BM280 device handle.
 * @return 0 on success, 1 if division by zero occurs.
 */
int bm280_convert_pressure(const bm280_handle_t handle)
{
    int64_t var1, var2, temp_pressure;
    
    var1 = ((int64_t)handle->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)handle->cal_val.press.dig_p6;
    var2 = var2 + ((var1*(int64_t)handle->cal_val.press.dig_p5) << 17);
    var2 = var2 + (((int64_t)handle->cal_val.press.dig_p4) << 35);
    var1 = ((var1 * var1 * (int64_t)handle->cal_val.press.dig_p3) >> 8) + ((var1 * (int64_t)handle->cal_val.press.dig_p2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)handle->cal_val.press.dig_p1) >> 33;

    if (var1 == 0) {
        BM280_LOG("Failed to convert pressure. Division by 0.\n");
        return 1;
    }

    temp_pressure = 1048576 - handle->press_raw;
    temp_pressure = (((temp_pressure << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)handle->cal_val.press.dig_p9) * (temp_pressure >> 13) * (temp_pressure >> 13) >> 25);
    var2 = (((int64_t)handle->cal_val.press.dig_p8) * temp_pressure) >> 19;

    temp_pressure = ((temp_pressure + var1 + var2) >> 8) + (((int64_t)handle->cal_val.press.dig_p7) << 4);
    handle->pressure = (int32_t)temp_pressure;

    return 0;
}

/**
 * @brief Converts the raw temperature value to readable format and calculates t_fine.
 * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf 
 * 
 * @param handle BM280 device handle.
 * @return 0 on success.
 */
int bm280_convert_temperature(const bm280_handle_t handle)
{
    int32_t var1, var2;

    var1 = ((((handle->temp_raw >> 3) - ((int32_t)handle->cal_val.temp.dig_t1 << 1))) * ((int32_t)handle->cal_val.temp.dig_t2)) >> 11;
    var2 = (((((handle->temp_raw >> 4) - ((int32_t)handle->cal_val.temp.dig_t1)) * ((handle->temp_raw >> 4) - ((int32_t)handle->cal_val.temp.dig_t1))) >> 12) * ((int32_t)handle->cal_val.temp.dig_t3)) >> 14;
    handle->t_fine = var1 + var2;

    handle->temperature = (handle->t_fine * 5 + 128) >> 8;

    return 0;
}

/**
 * @brief Converts the raw humidity value to readable format using calibration data.
 * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf 
 *
 * @param handle BM280 device handle.
 * @return 0 on success.
 */
int bm280_convert_humidity(const bm280_handle_t handle)
{
    int32_t v_x1;

    v_x1 = (handle->t_fine - ((int32_t)76800));
    v_x1 = (((((handle->hum_raw << 14) - (((int32_t)handle->cal_val.hum.dig_h4) << 20) - (((int32_t)handle->cal_val.hum.dig_h5) * v_x1)) 
            + ((int32_t)16384)) >> 15) * (((((((v_x1 * ((int32_t)handle->cal_val.hum.dig_h6)) >> 10) * (((v_x1 * ((int32_t)handle->cal_val.hum.dig_h3)) >> 11)
            + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)handle->cal_val.hum.dig_h2) + 8192) >> 14));
    v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((int32_t)handle->cal_val.hum.dig_h1)) >> 4));
    v_x1 = (v_x1 < 0 ? 0 : v_x1);
    v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
    handle->humidity = (uint32_t)(v_x1 >> 12);

    return 0;
}

/**
 * @brief Converts all raw sensor values (temperature, pressure, humidity).
 *
 * @param handle BM280 device handle.
 * @return 0 if all conversions succeed, otherwise 1.
 */
int bm280_convert_measurements(const bm280_handle_t handle)
{
    if (bm280_convert_temperature(handle) != 0) {
        BM280_LOG("Unable to convert temperature\n");
        return 1;
    }

    if (bm280_convert_pressure(handle) != 0) {
        BM280_LOG("Unable to convert pressure\n");
        return 1;
    }

    if (bm280_convert_humidity(handle) != 0) {
        BM280_LOG("Unable to convert humidity\n");
        return 1;
    }

    return 0;
}