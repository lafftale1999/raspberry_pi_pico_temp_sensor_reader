#include "../include/bm280_driver.h"
#include <stdio.h>
#include "pico/stdio.h"

// handle
struct bm280_handle{
    uint8_t temperature;
    uint8_t pressure;
    uint8_t humidity;
    uint8_t i2c_address;
};

// init

// read

// convert temp