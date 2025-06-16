/**
 * Copyright (C) 2025 Carl Broman
 *
 * @file        bm280_registers.h
 * @author      Carl Broman <carl.broman@yh.nackademin.se>
 * @brief       Register map for the BM280 sensor.
 * @note This should not be included directly - it is included through bm280.h
 * @addtogroup  bm280_registers BM280 Register Map
 * @{
 */

#ifndef BM280_REGISTERS_H_
#define BM280_REGISTERS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup BM280_registers_ID_and_reset ID and Reset
 * @ingroup bm280_registers
 * @brief Registers used for identification and reset.
 * @{
 */
#define BM280_ID_REG                    0xD0  /** Chip ID register (expected value: 0x60) */
#define BM280_RESET_REGISTER            0xE0  /** Reset register */
#define BM280_WRITE_RESET_BITS          0xB6  /** Command to trigger a software reset */
/** @} */

/**
 * @defgroup BM280_registers_humidity Humidity Control
 * @ingroup bm280_registers
 * @brief Humidity control register.
 */
#define BM280_CONTROL_HUMIDITY          0xF2  /** Controls humidity oversampling */

/**
 * @defgroup BM280_registers_temp_press_mode Temperature & Pressure Control
 * @ingroup bm280_registers
 * @brief Temperature, pressure control and mode setting register.
 * @details 
 * - Temperature: bits [5:7]  
 * - Pressure: bits [2:4]  
 * - Mode: bits [0:1]
 */
#define BM280_CONTROL_MEASURE           0xF4

/**
 * @defgroup BM280_read_data_registers Read Data Registers
 * @ingroup bm280_registers
 * @brief Registers for reading raw sensor measurement values.
 * @{
 */

#define BM280_PRESS_REG_MSB             0xF7
#define BM280_PRESS_REG_LSB             0xF8
#define BM280_PRESS_REG_XLSB            0xF9

#define BM280_TEMP_REG_MSB              0xFA
#define BM280_TEMP_REG_LSB              0xFB
#define BM280_TEMP_REG_XLSB             0xFC

#define BM280_HUMIDITY_REG_MSB          0xFD
#define BM280_HUMIDITY_REG_LSB          0xFE

#define BM280_READ_VALUES_REG_START     0xF7  /**< Starting address for bulk read */
#define BM280_READ_VALUES_REG_LEN       8     /**< Number of bytes to read all values */
/** @} */

/**
 * @defgroup BM280_config_registers Configuration Register
 * @ingroup bm280_registers
 * @brief Configuration register used for standby interval and IIR filter.
 * @details 
 * - Standby interval: bits [5:7]  
 * - IIR filter: bits [2:4]
 */
#define BM280_CONFIG_REGISTER           0xF5

/**
 * @defgroup BM280_calibration_registers Calibration Registers
 * @ingroup bm280_registers
 * @brief Registers holding factory calibration values.
 * @details 
 * Calibration registers are sequentially stored and grouped by measurement type.
 * @{
 */
#define BM280_TEMP_CALIB_PARAM_START    0x88
#define BM280_TEMP_PARAMS_LEN           6

#define BM280_PRESS_CALIB_PARAM_START   0x8E
#define BM280_PRESS_PARAMS_LEN          18

#define BM280_HUM_CALIB_FIRST           0xA1
#define BM280_HUM_CALIB_FIRST_LEN       1

#define BM280_HUM_CALIB_CONTINUE        0xE1
#define BM280_HUM_CALIB_CONTINUE_LEN    8

#define BM280_AMOUNT_CALIB_PARAMS \
    (BM280_TEMP_PARAMS_LEN + BM280_PRESS_PARAMS_LEN + \
     BM280_HUM_CALIB_FIRST_LEN + BM280_HUM_CALIB_CONTINUE_LEN)
/** @} */

#ifdef __cplusplus
}
#endif

#endif  // BM280_REGISTERS_H_
/** @} */  // End of bm280_registers