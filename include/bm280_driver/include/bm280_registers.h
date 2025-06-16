/**
 * Copyright (C) 2025 Carl Broman
 *
 * @file:       bm280_registers.h
 * @author:     Carl Broman <carl.broman@yh.nackademin.se>
 * @brief:  Registers and macros regarding the bm280 driver.
 * @addtogroup bm280_registers BM280 Register Map
 * @{
 -------------------------------------------------------------------------------------------------*/

#ifndef BM280_REGISTERS_H_
#define BM280_REGISTERS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup BM280_registers_ID_and_reset ID and Reset
 * @ingroup BM280_registers
 * @brief Registers used for identification and reset
 * @{
 */

/**
 * @brief Chip ID register
 * @details Reading this register returns the BM280 chip ID. Expected value: 0x60.
 */
#define BM280_ID_REG  0xD0

/**
 * @brief Reset register
 * @details Write BM280_WRITE_RESET_BITS (0xB6) to this register to perform a power-on reset.
 */
#define BM280_RESET_REGISTER  0xE0

/**
 * @brief Reset command value
 * @details Use this value when writing to BM280_RESET_REGISTER to trigger reset.
 */
#define BM280_WRITE_RESET_BITS  0xB6

/** @} */  // End of BM280_registers_ID_and_reset


/**
 * @defgroup BM280_registers_humidity Humidity Control
 * @ingroup BM280_registers
 * @brief Registers and macros used for humidity control
 * @{*/

/**
 * @brief Humidity control register
 * @details This register controls the oversampling rate of humidity data.
 */
#define BM280_CONTROL_HUMIDITY              0xF2

/**
 * @brief Humidity control macro
 * @details Macro for setting the correct oversampling
 * @param osrs_h Defined BM280_OSRS_XXX to represent the correct bits corresponding to the oversampling rates.
 */
#define BM280_SET_HUM_OSRS(osrs_h)          ((osrs_h) & 0x07)  

/** @} BM280_registers_humidity */ // End of BM280_registers_humidity

/**
 * @defgroup BM280_registers_temp_press_mode Temperature & Pressure Control, Set Mode
 * @ingroup BM280_registers
 * @brief Registers and macros used for controlling temperature and pressure, and setting the measurement mode of the BM280.
 * @{
*/

/**
 * @brief Temperature / Pressure control and Set Mode register.
 * @details The following part of the register controls the separate functions:
 * Temperature: BIT[5:7]
 * Pressure: BIT[2:4]
 * Reading Mode: BIT[0:1]
 */
#define BM280_CONTROL_MEASURE               0xF4

/**
 * @brief Temperature Control Macro
 * @details Macro for setting the correct oversampling
 * @param osrs_t Defined BM280_OSRS_XXX to represent the correct bits corresponding to the oversampling rates.
 */
#define BM280_SET_TEMP_OSRS(osrs_t)         (((osrs_t) & 0x07) << 5)

/**
 * @brief Pressure Control Macro
 * @details Macro for setting the correct oversampling
 * @param osrs_t Defined BM280_OSRS_XXX to represent the correct bits corresponding to the oversampling rates.
 */
#define BM280_SET_PRESS_OSRS(osrs_p)        (((osrs_p) & 0x07) << 2)

/**
 * @brief Set Mode macro
 * @details Macro for setting the correct pre-assigned values.
 * @param mode Use the Defined BM280_MODE_XXX to assign the correct values.
 */
#define BM280_SET_MODE(mode)                ((mode) & 0x03)

/**
 * @defgroup BM280_reading_modes Reading Modes
 * @ingroup BM280_registers_temp_press_mode
 * @brief The different reading modes for the BM280
 * @{
*/

/**
 * @brief Perpetual cycling of measurements and inactive periods.
*/
#define BM280_MODE_NORMAL                   0b11

/**
 * @brief No operation, all registers accessible, lowest power, selected after startup
*/
#define BM280_MODE_SLEEP                    0b00

/** 
 * @brief Perform one measurement, store results and return to sleep mode
*/
#define BM280_MODE_FORCED                   0b01
/**}@ */ // End of BM280_reading_modes
/**}@ */ // End of BM280_registers_temp_press

/** 
 * @defgroup BM280_read_data_registers Read Data Registers
 * @ingroup BM280_registers
 * @brief Registers for reading out the measurement values from the BM280
 * @{
*/

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

/**}@ */ // End of BM280_read_data_registers

/**
 * @defgroup BM280_config_registers Configuration Registers
 * @ingroup BM280_registers
 * @brief Registers for configurating the BM280
 * @{
*/

/**
 * @brief Register for configuration of measuring cycle interval and IIR Filter
 * @details Register is partioned as following:
 * Set interval: BIT[5:7]
 * Set IIR filter: BIT[2:4]
*/
#define BM280_CONFIG_REGISTER               0xF5

/**
 * @brief Macro for setting the standby interval.
 * @param ms Use the enum BM280_READ_INTERVALS_MS for the pre-defined values.
*/
#define BM280_SET_STANDBY_MS(ms)            (((ms) & 0x07) << 5)

/**
 * @brief Macro for setting the IIR filter
 * @details The IIR filter (Infinite Impulse Response) functions as a lowpass-filter to even out quick changes.
 * @param fs The filter setting from BM280_SET_FILTER_XXX 
*/
#define BM280_SET_IIR_FILTER(fs)            (((fs) & 0x07) << 2)
/**}@ */ // End of BM280_config_registers

#ifdef __cplusplus
}
#endif


#endif  // BM280_REGISTERS_H_
/** }@ */ // End of bm280_registers