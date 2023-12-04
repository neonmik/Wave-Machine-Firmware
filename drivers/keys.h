#pragma once


#include "../config.h"

#include "button.h"

#include "../synth/note_handling.h"

namespace KEYS {
    // Pin definitions for Mux
    constexpr   uint8_t     MUX_SEL_A   =   12;
    constexpr   uint8_t     MUX_SEL_B   =   13;
    constexpr   uint8_t     MUX_SEL_C   =   14;
    constexpr   uint8_t     MUX_SEL_D   =   15;
    constexpr   uint8_t     MUX_OUT_0   =   16;
    constexpr   uint8_t     MUX_OUT_1   =   17;

    // Key defines
    constexpr   uint8_t     MAX_KEYS    =   27;
    constexpr   uint8_t     PAGE_KEY    =   27;
    constexpr   uint8_t     LFO_KEY     =   28;
    constexpr   uint8_t     ARP_KEY     =   29;
    constexpr   uint8_t     PRESET_KEY  =   30;

    class Keyboard {
        private:


            uint32_t _history[8]    =       
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

            uint32_t    _current         =       0xFFFFFFFF;
            uint32_t    _last            =       0xFFFFFFFF;
            
            uint8_t     history_index;

            uint8_t _mux_address;

            
            inline void increment_mux_address (void) {
                // sets the index to loop
                _mux_address = (_mux_address + 1) % 16;
            }

            uint32_t reverse(uint32_t input) {
                uint32_t output = 0;
                for (int i = 0; i < 32; i++) {
                    if ((input & (1 << i))) output |= 1 << ((32 - 1) - i);
                }
                return output;
            }
        public:
            
            Keyboard() { }
            ~Keyboard() { }

            void init();
            void read();
            inline int32_t get() const {
                return _current;
            }
            inline int32_t get_last() const {
                return _last;
            }
            inline void set_last(uint32_t last) {
                _last = last;
            }

    };
    
    extern Keyboard Keys;

    void init (void);
    void read (void);
    void update (void);

    void setOctave (uint16_t input);
}
