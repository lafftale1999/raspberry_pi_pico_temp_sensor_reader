cmake_minimum_required(VERSION 3.13)

add_library(bm280_driver STATIC
    bm280.c
    internal/bm280_internal.c
)

target_include_directories(bm280_driver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Länka till Pico SDK-standardfunktioner
target_link_libraries(bm280_driver PUBLIC
    pico_stdlib
    hardware_i2c
)

# Om du använder t.ex. i2c_pico.h som ett eget bibliotek kan du lägga till:
# target_link_libraries(bm280_driver PUBLIC i2c_driver)

# Valfritt: exportera biblioteket för installation eller superbuilds
# export(TARGETS bm280_driver FILE bm280_driverConfig.cmake)