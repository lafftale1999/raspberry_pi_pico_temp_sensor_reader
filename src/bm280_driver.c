#include "../include/bm280_driver/bm280.h"
#include "../include/bm280_driver/include/bm280_json.h"
#include "../include/bm280_driver/include/bm280_registers.h"
#include "../include/bm280_driver/include/bm280_settings.h"
#include "../include/i2c_pico.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdio.h"
#include "pico/stdlib.h"

static bool json_parsed = false;

// handle
struct bm280_handle_internal{
    int32_t t_fine;         // variable needed for converting values correctly
    int32_t temperature;
    uint32_t pressure;
    uint32_t humidity;

    int32_t temp_raw;
    int32_t press_raw;
    int32_t hum_raw;

    uint8_t i2c_address;
    uint8_t address_length;

    char json_string[BM280_JSON_STR_MAX_LEN];
    
    // These are used for converting the measurements made by the sensor
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
 * @brief Parses the current sensor readings into a JSON-formatted string.
 *
 * @param handle Handle to the BM280 device.
 * @return Always returns 0. Updates internal `json_string` in handle.
 */
static int bm280_parse_json(bm280_handle_t handle) {
    json_parsed = true;
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
 * @return PICO_W_OK on success, PICO_W_FAIL on failure.
 */
static PICO_W_RETURN_STATUS bm280_calibration(bm280_handle_t handle) {
    
    uint8_t buf[BM280_AMOUNT_CALIB_PARAMS] = {0};
    
    uint8_t t_p_param_start = BM280_TEMP_CALIB_PARAM_START;
    // read temperature and pressure calibration values
    
    if (i2c_read_data(&handle->i2c_address, &t_p_param_start, buf, BM280_TEMP_PARAMS_LEN + BM280_PRESS_PARAMS_LEN) != PICO_W_OK) {
        printf("Unable to read temperature and pressure calibration values\n");
        return PICO_W_FAIL;
    }
    
    uint8_t hum_param[2] = {
        BM280_HUM_CALIB_FIRST,
        BM280_HUM_CALIB_CONTINUE
    };

    // read humidity calibration values
    if (i2c_read_data(&handle->i2c_address, &hum_param[0], &buf[24], BM280_HUM_CALIB_FIRST_LEN) != PICO_W_OK ||
        i2c_read_data(&handle->i2c_address, &hum_param[1], &buf[25], BM280_HUM_CALIB_CONTINUE_LEN) != PICO_W_OK) {
        printf("Unable to read humidity calibration values\n");
        return PICO_W_FAIL;
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

    return PICO_W_OK;
}

/**
 * @brief Converts the raw pressure value to readable format using calibration parameters.
 * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
 * 
 * @param handle BM280 device handle.
 * @return PICO_W_OK on success, PICO_W_FAIL if division by zero occurs.
 */
static PICO_W_RETURN_STATUS bm280_convert_pressure(bm280_handle_t handle) {
    int64_t var1, var2, temp_pressure;
    
    var1 = ((int64_t)handle->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)handle->cal_val.press.dig_p6;
    var2 = var2 + ((var1*(int64_t)handle->cal_val.press.dig_p5) << 17);
    var2 = var2 + (((int64_t)handle->cal_val.press.dig_p4) << 35);
    var1 = ((var1 * var1 * (int64_t)handle->cal_val.press.dig_p3) >> 8) + ((var1 * (int64_t)handle->cal_val.press.dig_p2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)handle->cal_val.press.dig_p1) >> 33;

    if (var1 == 0) {
        printf("Failed to convert pressure. Division by 0.\n");
        return PICO_W_FAIL;
    }

    temp_pressure = 1048576 - handle->press_raw;
    temp_pressure = (((temp_pressure << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)handle->cal_val.press.dig_p9) * (temp_pressure >> 13) * (temp_pressure >> 13) >> 25);
    var2 = (((int64_t)handle->cal_val.press.dig_p8) * temp_pressure) >> 19;

    temp_pressure = ((temp_pressure + var1 + var2) >> 8) + (((int64_t)handle->cal_val.press.dig_p7) << 4);
    handle->pressure = (int32_t)temp_pressure;

    return PICO_W_OK;
}

/**
 * @brief Converts the raw temperature value to readable format and calculates t_fine.
 * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf 
 * 
 * @param handle BM280 device handle.
 * @return PICO_W_OK on success.
 */
static PICO_W_RETURN_STATUS bm280_convert_temperature(bm280_handle_t handle) {
    int32_t var1, var2;

    var1 = ((((handle->temp_raw >> 3) - ((int32_t)handle->cal_val.temp.dig_t1 << 1))) * ((int32_t)handle->cal_val.temp.dig_t2)) >> 11;
    var2 = (((((handle->temp_raw >> 4) - ((int32_t)handle->cal_val.temp.dig_t1)) * ((handle->temp_raw >> 4) - ((int32_t)handle->cal_val.temp.dig_t1))) >> 12) * ((int32_t)handle->cal_val.temp.dig_t3)) >> 14;
    handle->t_fine = var1 + var2;

    handle->temperature = (handle->t_fine * 5 + 128) >> 8;

    return PICO_W_OK;
}

/**
 * @brief Converts the raw humidity value to readable format using calibration data.
 * @details Function provided by Bosch Sensortec in documentation for converting sensorvalues
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf 
 *
 * @param handle BM280 device handle.
 * @return PICO_W_OK on success.
 */
static PICO_W_RETURN_STATUS bm280_convert_humidity(bm280_handle_t handle) {
    int32_t v_x1;

    v_x1 = (handle->t_fine - ((int32_t)76800));
    v_x1 = (((((handle->hum_raw << 14) - (((int32_t)handle->cal_val.hum.dig_h4) << 20) - (((int32_t)handle->cal_val.hum.dig_h5) * v_x1)) 
            + ((int32_t)16384)) >> 15) * (((((((v_x1 * ((int32_t)handle->cal_val.hum.dig_h6)) >> 10) * (((v_x1 * ((int32_t)handle->cal_val.hum.dig_h3)) >> 11)
            + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)handle->cal_val.hum.dig_h2) + 8192) >> 14));
    v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((int32_t)handle->cal_val.hum.dig_h1)) >> 4));
    v_x1 = (v_x1 < 0 ? 0 : v_x1);
    v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
    handle->humidity = (uint32_t)(v_x1 >> 12);

    return PICO_W_OK;
}


/**
 * @brief Converts all raw sensor values (temperature, pressure, humidity).
 *
 * @param handle BM280 device handle.
 * @return PICO_W_OK if all conversions succeed, otherwise PICO_W_FAIL.
 */
static PICO_W_RETURN_STATUS bm280_convert_measurements(bm280_handle_t handle) {
    if (bm280_convert_temperature(handle) != PICO_W_OK) {
        printf("Unable to convert temperature\n");
        return PICO_W_FAIL;
    }

    if (bm280_convert_pressure(handle) != PICO_W_OK) {
        printf("Unable to convert pressure\n");
        return PICO_W_FAIL;
    }

    if (bm280_convert_humidity(handle) != PICO_W_OK) {
        printf("Unable to convert humidity\n");
        return PICO_W_FAIL;
    }

    return PICO_W_OK;
}

PICO_W_RETURN_STATUS bm280_read_data(bm280_handle_t handle) {

    uint8_t buf[BM280_READ_VALUES_REG_LEN];
    uint8_t register_start = BM280_READ_VALUES_REG_START;

    if(i2c_read_data(&handle->i2c_address, &register_start, buf, BM280_READ_VALUES_REG_LEN) != PICO_W_OK) {
        printf("Unable to read data from registers.\n");
        return PICO_W_FAIL;
    }
    handle->press_raw = (buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4); 
    handle->temp_raw = (buf[3] << 12) | (buf[4] << 4) | (buf[5] >> 4);
    handle->hum_raw = (buf[6] << 8) | buf[7];

    if(bm280_convert_measurements(handle) != PICO_W_OK) {
        printf("Unable to convert values.\n");
        return PICO_W_FAIL;
    }

    parse_json(handle);

    printf("Temp: %0.1fC | Humidity: %0.1f%% | Pressure: %0.1fhPa\n", (float)handle->temperature / 100.0f, (float)handle->humidity / 1024.0f, ((float)handle->pressure / 256.0f) / 100.0f);
 
    return PICO_W_OK;
}

PICO_W_RETURN_STATUS bm280_init(bm280_handle_t *handle, const uint8_t device_address, const BM280_READING_INTERVALS_MS interval) {
    bm280_handle_t p_handle = malloc(sizeof(struct bm280_handle_internal));

    if (p_handle == NULL) {
        printf("Unable to allocate handle on heap\n");
        goto exit;
    }

    p_handle->i2c_address = device_address;
    p_handle->address_length = BM280_ADDRESS_LENGTH;
    
    if (bm280_calibration(p_handle) != PICO_W_OK) {
        goto exit;
    }

    uint8_t humidity_set_up[2] = {
        BM280_CONTROL_HUMIDITY,
        BM280_SET_HUM_OSRS(BM280_OSRS_X1)
    };
    
    uint8_t measurement_set_up[3] = {
        BM280_CONTROL_MEASURE, 
        BM280_SET_TEMP_OSRS(BM280_OSRS_X2) | BM280_SET_PRESS_OSRS(BM280_OSRS_X16) | BM280_MODE_NORMAL
    };

    uint8_t config_set_up[2] = {
        BM280_CONFIG_REGISTER,
        BM280_SET_STANDBY_MS(interval) | BM280_SET_IIR_FILTER(BM280_SET_FILTER_16)
    };

    // Making sure that all configurations are set up correctly
    if (i2c_write_data(&p_handle->i2c_address, humidity_set_up, 2, true) != PICO_W_OK ||
        i2c_write_data(&p_handle->i2c_address, config_set_up, 2, true) != PICO_W_OK ||
        i2c_write_data(&p_handle->i2c_address, measurement_set_up, 2, false) != PICO_W_OK){
        printf("Unable to correctly set up configurations on bm280.\n");
        goto exit;
    }

    *handle = p_handle;

    return PICO_W_OK;

exit:
    printf("Initialization of bm280 failed\n");

    if (p_handle != NULL) {
        free(p_handle);
    }

    return PICO_W_FAIL;
}

const char* BM280_get_json(bm280_handle_t handle) {
    if (json_parsed){
        return handle->json_string;
    }
    else {
        return NULL;
    }
}