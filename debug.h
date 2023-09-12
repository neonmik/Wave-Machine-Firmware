#pragma once

#include "pico/stdlib.h"

#ifdef ENABLE_DEBUG
#define DebugPrint(message) \
    do { \
        printf("[DEBUG] %s\n", message); \
    } while (0)
#else
#define DebugPrint(message)
#endif


namespace DEBUG {
    // create some kind of debug-level system here
    enum Mode {
        OFF,
        PRINT,
        BREAKPOINT
    };

    inline void overflow (void) {
        printf("OVERFLOW! ***Generic Debug Warning***\n");
    }
    inline void breakpoint (void) {
        __asm__("bkpt #0");
    } 
}