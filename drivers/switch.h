#pragma once

#define LONG_PRESS      400

#include "pico/stdlib.h"
#include "pico/time.h"

namespace switch {
    enum State {
        DOUBLE  = 64,
        SHIFT   = 32,
        LONG    = 16,
        SHORT   = 8,
        TOGGLE  = 1
    }
    class Switch
    {
    private:
        bool        shift_flag    = 0;
        bool        short_flag    = 0;
        bool        long_flag    = 0;
        bool        double_flag    = 0;
        bool        state  = 0;
        uint32_t    start  = 0;
        uint32_t    end    = 0;

    public:

        Switch(/* args */);
        ~Switch();

        void init (State state);
        void start (void);
        void stop (void);
        bool get (void);
    };
}


