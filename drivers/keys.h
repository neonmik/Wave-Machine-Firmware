#pragma once


#include "../config.h"

#include "mux.h"

#include "button.h"

#include "../synth/note_handling.h"



namespace KEYS {
    namespace {
        uint32_t readBuffer = 0;

        uint32_t history[8]    =       {
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
        };

        uint32_t    current         =       0xFFFFFFFF;
        uint32_t    last            =       0xFFFFFFFF;
        
        uint8_t     historyIndex;

        uint8_t     lastAddress    =       0;

        // uint8_t note_state[128] = {0};

        
        uint32_t reverse(uint32_t input) {
            uint32_t output = 0;
            for (int i = 0; i < 32; i++) {
                if ((input & (1 << i))) output |= 1 << ((32 - 1) - i);
            }
            return output;
        }

        inline int32_t get() {
            return current;
        }
        inline int32_t getLast() {
            return last;
        }
        inline void setLast(uint32_t input) {
            last = input;
        }

        void pinInit (uint8_t pin) {
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_IN);
            gpio_pull_up(pin); // already doing in hardware, but just in case...
        }
    }

    void init (void);
    void read (void);
    void update (void);

    void printBuffer(void);

    void setOctave (uint16_t input);
}
