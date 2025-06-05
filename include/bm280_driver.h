#ifndef BM280_DRIVER_H
#define BM280_DRIVER_H

// *********************************
// ******** CONFIGURATIONS *********
// *********************************
#define BM280_UNITS_ON_BUS                  1           // The amount of BM280 units on the I2C bus.
#define BM280_ADDRESSES_ON_BUS              {0x76}      // Array of addresses for BM280 units on the I2C bus.
#define BM280_ADDRESS_LENGTH                7           // Length of address in bits.

// OVERSAMPLING RATES
#define BM280_OSRS_SKIPPED                  0b000
#define BM280_OSRS_X1                       0b001
#define BM280_OSRS_X2                       0b010
#define BM280_OSRS_X4                       0b011
#define BM280_OSRS_X8                       0b100
#define BM280_OSRS_X16                      0b101    

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
#define BM280_SET_HUM_OSRS(osrs_h)          ((osrs_h) & 0x07)

/* TEMPERATURE & PRESSURE
    BM280_SET_TEMP_OSRS(BM280_OSRS_XN) to set temperature OSRS
    BM280_SET_PRESS_OSRS(BM280_OSRS_XN) to set pressure OSRS
    BM280_SET_MODE(BM280_MODE_XXX) to set mode
*/
#define BM280_CONTROL_MEASURE               0xF4
#define BM280_SET_TEMP_OSRS(osrs_t)         (((osrs_t) & 0x07) << 5)
#define BM280_SET_PRESS_OSRS(osrs_p)        (((osrs_t) & 0x07) << 2)

#define BM280_MODE_NORMAL                   0b11                // perpetual cycling of measurements and inactive periods.
#define BM280_MODE_SLEEP                    0b00                // no operation, all registers accessible, lowest power, selected after startup
#define BM280_MODE_FORCED                   0b01                // perform one measurement, store results and return to sleep mode
#define BM280_SET_MODE(mode)                ((mode) & 0x03)

struct bm280_handle *bm280_handle_t;

#endif