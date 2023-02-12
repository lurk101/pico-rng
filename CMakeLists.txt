cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(rndtest C CXX ASM)

pico_sdk_init()

add_executable(rndtest rndtest.c)

pico_set_program_name(rndtest "rndtest")
pico_set_program_version(rndtest "0.2")

pico_enable_stdio_uart(rndtest 1)
pico_enable_stdio_usb(rndtest 0)

target_link_libraries(rndtest pico_stdlib pico_rand)

pico_add_extra_outputs(rndtest)

