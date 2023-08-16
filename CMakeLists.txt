cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(wavemachine-firmware-v0.29 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()



add_executable(${PROJECT_NAME}
    main.cc

    usb_descriptors.c
    drivers/usb.cc
    
    drivers/adc.cc
    drivers/button.cc
    drivers/dac.cc
    drivers/eeprom.cc
    drivers/keys.cc
    drivers/leds.cc
    drivers/rgb.cc
    drivers/shiftreg.cc
    drivers/uart.cc
    
    
    ui.cc
    pagination.cc
    
    settings.cc
    functions.cc
    queue.cc
    
    random.cc
    
    midi.cc
    
    synth/adsr.cc
    synth/arp.cc
    synth/clock.cc
    synth/controls.cc
    synth/filter.cc
    synth/fx.cc
    synth/modulation.cc
    synth/note_handling.cc
    synth/resources.cc
    synth/synth.cc
    
    )

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        )

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/drivers/sn74595.pio)

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
                        hardware_pio
                        hardware_timer

                        tinyusb_device
                        tinyusb_board
                        )

# add_compile_options(-CFG_TUSB_CONFIG_FILE=${CMAKE_CURRENT_LIST_DIR}/drivers/tusb_config.h)