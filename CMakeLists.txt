cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(tangle C CXX ASM)

pico_sdk_init()

add_executable(tangle tangle.cpp)

pico_set_program_name(tangle "tangle")
pico_set_program_version(tangle "0.2")

pico_enable_stdio_uart(tangle 1)
pico_enable_stdio_usb(tangle 0)

target_link_libraries(tangle pico_stdlib)

pico_add_extra_outputs(tangle)

