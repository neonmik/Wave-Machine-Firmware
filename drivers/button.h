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
            bool        short_flag_     = 0;
            bool        long_flag_      = 0;
            bool        double_flag_    = 0;
            bool        shift_flag_     = 0;


            bool        state_          = 0;
            bool        last_state_     = 0;

            uint32_t    start_          = 0;
            uint32_t    end_            = 0;
            uint32_t    released_       = 0;
            uint8_t     counter_        = 0;

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