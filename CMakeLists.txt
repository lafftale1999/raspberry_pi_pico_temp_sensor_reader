# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(raspberry_pico_w_bme280_i2c C CXX ASM)

set(PICO_USE_MBEDTLS ON)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(bm280_driver)

# Add executable. Default name is the project name, version 0.1
FILE(GLOB SRC_FILES src/*.c)
add_executable(raspberry_pico_w_bme280_i2c
    main.c
    ${SRC_FILES})

pico_set_program_name(raspberry_pico_w_bme280_i2c "raspberry_pico_w_bme280_i2c")
pico_set_program_version(raspberry_pico_w_bme280_i2c "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(raspberry_pico_w_bme280_i2c 0)
pico_enable_stdio_usb(raspberry_pico_w_bme280_i2c 1)

# Lägg till mbedtls och stäng av plattformsberoende entropi
add_subdirectory(${PICO_SDK_PATH}/lib/mbedtls ${CMAKE_BINARY_DIR}/mbedtls EXCLUDE_FROM_ALL)
target_compile_definitions(mbedcrypto PRIVATE MBEDTLS_NO_PLATFORM_ENTROPY)
target_compile_definitions(raspberry_pico_w_bme280_i2c PRIVATE MBEDTLS_NO_PLATFORM_ENTROPY)

# Add the standard library to the build
target_link_libraries(raspberry_pico_w_bme280_i2c
        pico_stdlib
        hardware_i2c
        pico_cyw43_arch_lwip_threadsafe_background
        pico_lwip_mqtt
        pico_lwip_mbedtls
        pico_mbedtls
        bm280_driver
        )

# Add the standard include files to the build
target_include_directories(raspberry_pico_w_bme280_i2c PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${PICO_SDK_PATH}/lib/mbedtls/include
        ${CMAKE_CURRENT_LIST_DIR}/bm280_driver
)

# Add any user requested libraries
target_link_libraries(raspberry_pico_w_bme280_i2c 
        hardware_i2c
        pico_cyw43_arch_lwip_threadsafe_background
        )

pico_add_extra_outputs(raspberry_pico_w_bme280_i2c)