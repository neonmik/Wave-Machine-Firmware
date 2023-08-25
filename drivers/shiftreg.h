#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
// #include "hardware/timer.h"

#include "../debug.h"


enum SRPins : uint8_t {
        SR_DATA     = 18,
        SR_CLK      = 19,
        SR_LATCH    = 20,
        SR_DATA_WIDTH = 8
};

enum Pins : uint8_t {
        ALL     = 255,
        KNOBS   = 15,
        KNOB_1  = 1,
        KNOB_2  = 2,
        KNOB_3  = 4,
        KNOB_4  = 8,
        PAGES   = 112,
        PAGE_1  = 16,
        PAGE_2  = 32,
        PAGE_3  = 64,
        SPARE   = 128,
        OFF     = 0
};


namespace ShiftReg {
    namespace {
        uint8_t     _buffer[8];
        uint8_t     time            = 0;
        uint16_t    resolution      = 8;
        bool        needsSending;

        void tick (void) {
            ++time;
            if (time == resolution) {
                time = 0;
                needsSending = true;
            }
        }
    }

    void Init();
    void Update (void);
    void set_bit(Pins pin, bool value);
    void clear (void);

}
