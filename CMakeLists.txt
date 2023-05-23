cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(beepmachine-firmware-v0.26 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.cc

    
    drivers/adc.cc
    drivers/button.cc
    drivers/dac.cc
    drivers/eeprom.cc
    drivers/keys.cc
    drivers/leds.cc
    drivers/rgb.cc
    drivers/shiftreg.cc

    ui.cc
    pagination.cc

    mailbox.cc
    random.cc
    functions.cc

    synth/adsr.cc
    synth/arp.cc
    synth/beat_clock.cc
    # synth/filter.cc
    synth/modulation.cc
    synth/note_priority.cc
    # synth/oscillator.cc
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
                        hardware_i2c
                        # hardware_flash
                        )