#pragma once

#include "pico/stdlib.h"

namespace DEBUG {
    // create some kind of debug-level system here
    enum Mode {
        OFF,
        PRINT,
        BREAKPOINT
    };
    inline void overflow (void) {
        printf("OVERFLOW! ***Generic Debug Warning***");
    }
    inline void breakpoint (void) {
        __asm__("bkpt #0");
    } 
}