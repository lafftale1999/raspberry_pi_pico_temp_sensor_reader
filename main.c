#include "include/i2c_external.h"
#include "include/bm280_driver.h"

int main()
{
    stdio_init_all();
    sleep_ms(5000);

    printf("Start I2C init\n");
    if (i2c_open() == PICO_W_OK) {
        printf("i2c init succesful!\n");
    }

    bm280_handle_t handle = NULL;
    if (bm280_init(&handle, 0x76, INTERVAL_1000MS) == PICO_W_OK) {
        printf("device succesfully init\n");
    }
    
    while(1) {
        bm280_read_data(handle);
        sleep_ms(5000);
    }
}