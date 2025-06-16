/**
 * Copyright (C) 2025 Carl Broman
 *
 * @file:       bm280_json.h
 * @author:     Carl Broman <carl.broman@yh.nackademin.se
 * @brief:  Json headerfile for correctly parsing bm280 values
 * @addtogroup bm280_json BM280_JSON
 * @{
 * @brief Headerfile for defines regarding parsing BM280 values into JSON strings.
 * @details Headerfile for defines regarding parsing BM280 values into JSON strings.
 -------------------------------------------------------------------------------------------------*/

#ifndef BM280_JSON_H_
#define BM280_JSON_H_

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @defgroup BM280_json_defines BM280 JSON defines
 * @brief Definitions for parson the BM280 values to a JSON string
 * @{
*/
#define BM280_JSON_TEMP_KEY                 "temperature"
#define BM280_JSON_PRESS_KEY                "pressure"
#define BM280_JSON_HUM_KEY                  "humidity"
#define BM280_JSON_KEYS                     {BM280_JSON_TEMP_KEY, BM280_JSON_HUM_KEY, BM280_JSON_PRESS_KEY}
#define BM280_JSON_KEYS_LEN                 20
#define BM280_JSON_KEYS_SIZE                3
#define BM280_JSON_VALS_LEN                 10
#define BM280_JSON_STR_MAX_LEN              ((BM280_JSON_KEYS_LEN + BM280_JSON_VALS_LEN + 5) * BM280_JSON_KEYS_SIZE)
#define BM280_JSON_BUF_SIZE                 64
/** @} BM280_json_defines */

#ifdef __cplusplus
}
#endif

#endif  // BM280_JSON_H_
/** @} BM280_JSON */
