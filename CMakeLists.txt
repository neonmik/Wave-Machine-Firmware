cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(beepmachine-firmware-v0.25 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.cc

    drivers/dac.cc

    drivers/adc.cc
    drivers/keys.cc
    drivers/button.cc
    drivers/leds.cc
    drivers/shiftreg.cc
    drivers/rgb.cc

    ui.cc
    pagination.cc

    mailbox.cc

    synth/adsr.cc
    synth/arp.cc
    synth/beat_clock.cc
    synth/modulation.cc
    synth/note_priority.cc
    synth/settings.cc
    synth/synth.cc
)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} 
                        pico_stdlib
                        pico_bootrom
                        pico_multicore
                        hardware_pwm
                        hardware_spi
                        hardware_dma
                        hardware_irq
                        hardware_adc
                        hardware_flash
                        )