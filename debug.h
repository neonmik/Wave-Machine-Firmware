#pragma once

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
    
    inline void print (const char* message) {
        printf("[DEBUG]: %s\n", message);
    }
    inline void error (const char* message) {
        printf("[ERROR]: %s\n", message);
    }
    inline void warning (const char* message) {
        printf("[WARNING]: %s\n", message);
    }

    inline void overflow (void) {
        printf("OVERFLOW! ***Generic Debug Warning***\n");
    }
    inline void breakpoint (void) {
        __asm__("bkpt #0");
    } 
}