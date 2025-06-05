#include "../include/i2c_internal.h"
#include "../include/i2c_external.h"

PICO_W_RETURN_STATUS i2c_open() {
    _i2c_init();
}