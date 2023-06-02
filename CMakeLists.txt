cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(servo_test C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(servo_test
    main.c
)

pico_add_extra_outputs(servo_test)

target_include_directories(servo_test INTERFACE ./)

target_link_libraries(servo_test pico_stdlib hardware_pwm hardware_adc)

