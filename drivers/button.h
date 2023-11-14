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
            
            bool        shortPress      = false;
            bool        longPress       = false;
            bool        doublePress     = false;
            bool        shiftPress      = false;

            uint32_t    start           = 0;
            uint32_t    end             = 0;
            uint32_t    lastTime        = 0;

        public:

            Button() { }
            ~Button() { }

            void pressed (void);
            void released (void);

            bool get (State state);
    };

    extern Button PAGE;
    extern Button FUNC1;
    extern Button FUNC2;
    extern Button PRESET;
}