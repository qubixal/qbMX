cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(qbmx_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(qbmx
    src/main.c
    src/matrix.c
    src/usb_descriptors.c
    src/oled.c
    src/leds.c
    src/encoder.c
)

target_include_directories(qbmx PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/includes
    ${CMAKE_CURRENT_LIST_DIR}/fonts
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(qbmx
    pico_stdlib
    pico_time
    pico_sync
    hardware_adc
    hardware_i2c
    hardware_pio
    hardware_timer
    hardware_gpio
    hardware_clocks
    tinyusb_device
    tinyusb_common
)

pico_add_extra_outputs(qbmx)
pico_enable_stdio_usb(qbmx 1)
pico_enable_stdio_uart(qbmx 0)
