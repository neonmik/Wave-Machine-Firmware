#pragma once

#include "../config.h"

#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

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

#define     MAX_RESOLUTION  64
namespace ShiftReg {
    namespace {
        uint8_t     buffer;
        uint8_t     time            = 0;
        uint8_t     count           = 1;
        uint8_t     *data;

        bool        needsSending;

        int         shiftRegPWM;

    }

    void init(void);
    void update (void);
    void set_bit(Pins pin, bool value);
    void clear (void);

}
