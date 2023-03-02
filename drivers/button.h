#pragma once

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#define LONG_PRESS_TIME     400
#define DOUBLE_PRESS_TIME   150

namespace Buttons {
    enum class ButtonState {
        SHORT,
        LONG,
        DOUBLE,
        SHIFT
    };
        
    class Button {
        private:
            
            bool        _short          = 0;
            bool        _long           = 0;
            bool        _double         = 0;
            bool        _shift          = 0;


            // bool        _state          = 0;
            // bool        _last_state     = 0;

            uint32_t    _start          = 0;
            uint32_t    _end            = 0;
            uint32_t    _released       = 0;
            uint8_t     _counter        = 0;
            uint32_t    _last_press_time = 0;

        public:

            Button() { }
            ~Button() { }

            void pressed (void);
            void released (void);

            bool get (ButtonState state);
    };

    extern Button PAGE;
    extern Button LFO;
    extern Button ARP;
    extern Button PRESET;
}