/**
 * Copyright (C) 2025 Carl Broman
 *
 * @file:       bm280_settings.h
 * @author:     Carl Broman <carl.broman@yh.nackademin.se>
 * @brief:  The different setting available for the BM280.
 * @note This should not be included directly - it is included through bm280.h
 * @defgroup bm280_settings BM280 Settings
 * @ingroup bm280_driver_internal
 * @{
 * @details Macros and Defines for configuring the BM280 sensor.
 */

#ifndef BM280_SETTINGS_H_
#define BM280_SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/
#include "bm280_registers.h"

/**
 * @defgroup BM280_measurement_settings Measurements Settings
 * @ingroup BM280_settings
 * @brief The settings available for Oversampling and IIR filter
 * @{
*/
    /**
     * @fn BM280_SET_STANDBY_MS(ms)
     * @brief Macro for setting the standby interval. This will decide for how long the
     * BM280 will sleep between taking measurements. (Only effective in BM280_MODE_NORMAL)
     * 
     * This macro generate a bitfield for use in writing to BM280 configuration register.
     * Use the following values:
     * - INTERVAL_0_5MS
     * - INTERVAL_62_5MS
     * - INTERVAL_125MS
     * - INTEVAL_250MS
     * - INTERVAL_500MS
     * - INTERVAL_1000MS
     * - INTERVAL_10MS
     * - INTERVAL_20MS
     * 
     * @param ms Use the enum BM280_READ_INTERVALS_MS for the pre-defined values
     * @return Bitfield for BM280_CONFIG_REGISTER
    */
    #define BM280_SET_STANDBY_MS(ms)            ((((ms) & 0x07) << 5) | BM280_CONFIG_REGISTER)

    /**
     * @fn BM280_SET_IIR_FILTER(fs)
     * @brief Macros to configure temperature, pressure, and humidity oversampling.
     *
     * This macros generate a bitfield for use in writing to BM280 control registers.
     * Use the following values for oversampling:
     *   - BM280_SET_FILTER_OFF
     *   - BM280_SET_FILTER_2
     *   - BM280_SET_FILTER_4
     *   - BM280_SET_FILTER_8
     *   - BM280_SET_FILTER_16
     *
     * @param fs Filter settings from the predefined BM280_SET_FILTER_XXX
     * @return Bitfield for BM280_CONFIG_REGISTER
     */
    #define BM280_SET_IIR_FILTER(fs)            ((((fs) & 0x07) << 2) | BM280_CONFIG_REGISTER)

    /**
     * @defgroup BM280_oversampling_macros Oversampling Macros
     * @ingroup BM280_measurement_settings
     * @brief Macros to configure temperature, pressure, and humidity oversampling.
     *
     * These macros generate bitfields for use in writing to BM280 control registers.
     * Use the following values for oversampling:
     *   - BM280_OSRS_SKIPPED
     *   - BM280_OSRS_X1
     *   - BM280_OSRS_X2
     *   - BM280_OSRS_X4
     *   - BM280_OSRS_X8
     *   - BM280_OSRS_X16
     *
     * @note All macros return a bitfield; they do not write directly to registers.
     * @{
     */

    #define BM280_SET_TEMP_OSRS(osrs_t)         ((((osrs_t) & 0x07) << 5) | BM280_CONTROL_MEASURE) /**< Macro to configure temperature oversampling. */
    #define BM280_SET_PRESS_OSRS(osrs_p)        ((((osrs_p) & 0x07) << 2) | BM280_CONTROL_MEASURE) /**< Macro to configure pressure oversampling. */
    #define BM280_SET_HUM_OSRS(osrs_h)          (((osrs_h) & 0x07) | BM280_CONTROL_HUMIDITY) /**< Macro to configure humidity oversampling. */

    /** @} */ // End of BM280_oversampling_macros

    /**
     * @defgroup BM280_osrs Oversampling Rates
     * @ingroup BM280_measurement_settings
     * @brief
     * Predefined constants for oversampling settings in the BM280 sensor.
     *
     * These values are used in macros such as:
     * - BM280_SET_TEMP_OSRS()
     * - BM280_SET_PRESS_OSRS()
     * - BM280_SET_HUM_OSRS()
     *
     * Meaning of constants:
     * - SKIPPED: no measurement
     * - X1 to X16: increasing oversampling rate and accuracy
     *
     * @{
     */
    #define BM280_OSRS_SKIPPED      0b000  /**< Skip measurement (power saving) */
    #define BM280_OSRS_X1           0b001  /**< Oversampling x1 */
    #define BM280_OSRS_X2           0b010  /**< Oversampling x2 */
    #define BM280_OSRS_X4           0b011  /**< Oversampling x4 */
    #define BM280_OSRS_X8           0b100  /**< Oversampling x8 */
    #define BM280_OSRS_X16          0b101  /**< Oversampling x16 */

    /** @} */ // End of BM280_osrs


    /**
     * @defgroup BM280_iir IIR Settings
     * @ingroup BM280_measurement_settings
     * @brief
     * Predefined constants for IIR settings in the BM280 sensor.
     * 
     * These values are used in the macro:
     * - BM280_SET_IIR_FILTER()
     * 
     * Meaning of constants:
     * - OFF: No filter (Very sensitivte)
     * - X1 to X16: increasing stability
     * @{
    */
    #define BM280_SET_FILTER_OFF    0b000 /**< Filter off, very sensitive to changes */
    #define BM280_SET_FILTER_2      0b001 /**< Filter 2 */
    #define BM280_SET_FILTER_4      0b010 /**< Filter 4 */
    #define BM280_SET_FILTER_8      0b011 /**< Filter 8 */
    #define BM280_SET_FILTER_16     0b100 /**< Filter 16 */

    /**@}*/ // END OF BM280_iir

/**@} */ // END OF BM280_measurement_settings

/**
 * @defgroup BM280_reading_modes Reading Modes
 * @ingroup BM280_settings
 * @brief The different reading modes for the BM280. Coupled together with SET_MODE
 * @{
*/
    /**
     * @fn BM280_SET_MODE(mode)
     * @brief Macro for setting the mode on the BM280
     * 
     * This macro generate a bitfield for use in writing to the BM280 Control Measure Setting.
     * Use the following values for setting the mode:
     * - BM280_MODE_NORMAL
     * - BM280_MODE_SLEEP
     * - BM280_MODE_FORCED
     * 
     * @param mode Use the Defined BM280_MODE_XXX to assign the correct values.
     * @return Bitfield to be written into register.
     */
    #define BM280_SET_MODE(mode)                (((mode) & 0x03) | BM280_CONTROL_MEASURE)

    #define BM280_MODE_NORMAL                   0b11    /**< Perpetual cycling of measurements and inactive periods. */
    #define BM280_MODE_SLEEP                    0b00    /**< No operation, all registers accessible, lowest power, selected after startup */
    #define BM280_MODE_FORCED                   0b01    /**< Perform one measurement, store results and return to sleep mode */

/**}@ */ // End of BM280_reading_modes
#ifdef __cplusplus
}
#endif
/**@} */ // End of bm280_settings 
#endif  // BM280_SETTINGS_H_
