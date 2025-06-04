#include <stdio.h>
#include "pico/stdlib.h"
#include "include/i2c_driver.h"

int main()
{
    stdio_init_all();
    sleep_ms(10000);

    printf("Start I2C init\n");
    if (_i2c_init() == PICO_W_OK) {
        printf("i2c init succesful!\n");
    }
    
    while(1) {
        scan_i2c_bus();
        
        printf("Hello World!\n");
        sleep_ms(1000);
    }
    
}