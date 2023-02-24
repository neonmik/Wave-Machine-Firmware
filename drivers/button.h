#pragma once

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#define LONG_PRESS      400
#define DOUBLE_PRESS    200


namespace Buttons {
    enum {
        DOUBLE  = 64,
        SHIFT   = 32,
        LONG    = 16,
        SHORT   = 8,
        TOGGLE  = 1
    };
        
    class Button {
        private:
            bool        _short          = 0;
            bool        _long           = 0;
            bool        _double         = 0;
            bool        _shift          = 0;


            bool        _state          = 0;
            bool        _last_state     = 0;

            uint32_t    _start          = 0;
            uint32_t    _end            = 0;
            uint32_t    _released       = 0;
            uint8_t     _counter        = 0;

        public:

            Button() { }
            ~Button() { }

            void pressed (void);
            void released (void);

            bool get_short (void);
            bool get_long (void);
            bool get_double (void);
            bool get_shift (void);
    };

    extern Button PAGE;
    extern Button LFO;
    extern Button ARP;
    extern Button PRESET;
}