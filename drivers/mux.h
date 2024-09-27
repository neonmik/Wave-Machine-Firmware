#pragma once

#include "../config.h"
#include "../functions.h"

namespace MUX {

    constexpr   uint8_t     MUX_SEL_A   =   12;
    constexpr   uint8_t     MUX_SEL_B   =   13;
    constexpr   uint8_t     MUX_SEL_C   =   14;
    constexpr   uint8_t     MUX_SEL_D   =   15;

    namespace {
        uint8_t address = 0;

        void pinInit (uint8_t pin) {
            gpio_init(pin);
            // set the pins direction
            gpio_set_dir(pin, GPIO_OUT);

            // set the slew rate slow (for reducing amount of cross talk on address changes...)
            gpio_set_slew_rate(pin, GPIO_SLEW_RATE_SLOW);

            gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_2MA);
        }

        void transferAddress (void) {
            gpio_put(MUX_SEL_A, address & 1); 
            gpio_put(MUX_SEL_B, (address >> 1) & 1);
            gpio_put(MUX_SEL_C, (address >> 2) & 1);
            gpio_put(MUX_SEL_D, (address >> 3) & 1);
        }
    }

    void    init (void);
    void    incrementAddress(void);
    void    setAddress (uint8_t newAddress);
    uint8_t getAddress (void);
}