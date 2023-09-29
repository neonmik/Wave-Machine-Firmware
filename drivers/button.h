#pragma once

#include "../config.h"

#include "pico/time.h"

#define LONG_PRESS_TIME     350
#define DOUBLE_PRESS_TIME   150

namespace Buttons {
    enum class State {
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

            bool get (State state);
    };

    extern Button PAGE;
    extern Button LFO;
    extern Button ARP;
    extern Button PRESET;
}