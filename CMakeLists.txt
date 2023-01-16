cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(beepmachine-firmware-v0.22 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.cc
    synth.cc
    drivers/dac.cc
    drivers/adc.cc
    drivers/keys.cc
    # drivers/switch.cc
    hardware.cc
    synth/modulation.cc
)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} 
                        pico_stdlib
                        hardware_pwm
                        hardware_spi
                        hardware_dma
                        hardware_irq
                        hardware_adc
                        )