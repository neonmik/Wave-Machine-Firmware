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


cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(wavemachine LANGUAGES C CXX ASM VERSION 0.33)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()




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
    
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}  "${PROJECT_BINARY_DIR}"
    )
    
configure_file(version.h.in version.h @ONLY)
    
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/drivers/sn74595.pio)
    
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

pico_add_extra_outputs(${PROJECT_NAME})

# target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_DEBUG)  # add this line to enable debug output


target_link_libraries(${PROJECT_NAME} 
                        pico_stdlib
                        pico_bootrom
                        pico_multicore
                        pico_unique_id
                        
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

add_compile_options(- cmake -DPICO_NO_FLASH=1)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  PICO_DEFAULT_UART=0
  PICO_DEFAULT_UART_TX_PIN=4
  PICO_DEFAULT_UART_RX_PIN=5
)