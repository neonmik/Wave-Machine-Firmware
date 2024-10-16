# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pimoroni_pico_plus2_rp2350 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(wavemachine LANGUAGES C CXX ASM VERSION 0.33)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add the version header file
configure_file(version.h.in version.h @ONLY)

# Add executable. Default name is the project name, version 0.1

add_executable(${PROJECT_NAME} 
    main.cc

    settings.cc
    functions.cc
    random.cc

    usb_descriptors.c
    drivers/usb.cc

    drivers/adc.cc
    drivers/button.cc
    drivers/dac.cc
    drivers/eeprom.cc
    drivers/interpolation.cc
    drivers/keys.cc
    drivers/leds.cc
    drivers/mux.cc
    drivers/rgb.cc
    drivers/shiftreg.cc
    drivers/uart.cc
    
    
    ui.cc
    pagination.cc
    controls.cc
    preset.cc
    interval.cc
    
    queue.cc
    
    midi.cc
    
    synth/adsr.cc
    synth/arp.cc
    synth/clock.cc
    synth/filter.cc
    synth/fx.cc
    synth/modulation.cc
    synth/note_handling.cc
    synth/resources.cc
    synth/synth.cc
    
    msc_disk.cc
    )
    
# Generate PIO header
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/drivers/sn74595.pio)
    
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PICO_DEFAULT_UART=1
    PICO_DEFAULT_UART_TX_PIN=4
    PICO_DEFAULT_UART_RX_PIN=5
)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 0)

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(${PROJECT_NAME} 
            pico_stdlib
            pico_bootrom
            pico_multicore
            pico_unique_id
            pico_stdio_uart

            hardware_pwm
            hardware_spi
            hardware_dma
            hardware_irq
            hardware_adc
            hardware_i2c
            hardware_pio
            hardware_timer
            hardware_interp

            tinyusb_device
            tinyusb_board
        )

pico_add_extra_outputs(${PROJECT_NAME})

# add_compile_options(- cmake -DPICO_NO_FLASH=1)
