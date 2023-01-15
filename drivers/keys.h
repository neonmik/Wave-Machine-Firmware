#pragma once

#define MUX_SEL_A       12
#define MUX_SEL_B       13
#define MUX_SEL_C       14
#define MUX_SEL_D       15
#define MUX_OUT_0       16
#define MUX_OUT_1       17

#define MAX_KEYS        27
#define PAGE_KEY        27
#define LFO_KEY         28
#define ARP_KEY         29
#define PRESET_KEY      30

#define DEBUG_PIN       2

#define LONG_PRESS      400

#include "pico/stdlib.h"

class Keys {
    private:
        uint32_t keys_history[8]    =       
        {
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
            0xFFFFFFFF,
        };

        uint32_t keys               =       0xFFFFFFFF;
        uint32_t keys_last          =       0xFFFFFFFF;
        uint8_t num_keys_down       =       0;
        uint8_t history_index;
        inline uint32_t reverse(uint32_t input) {
            uint32_t output = 0;
            for (int i = 0; i < 32; i++) {
                if ((input & (1 << i))) output |= 1 << ((32 - 1) - i);
            }
            return output;
        }
    public:
        Keys() { }
        ~Keys() { }

        void init();
        void read();
        inline int32_t get() const {
            return keys;
        }
        inline int32_t get_last() const {
            return keys_last;
        }
        inline void set_last(uint32_t last) {
            keys_last = last;
        }

};






