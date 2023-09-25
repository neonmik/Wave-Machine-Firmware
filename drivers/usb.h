#pragma once

#include "pico/stdlib.h"

#include "bsp/board.h"
#include "tusb.h"

namespace USB {

    constexpr   uint8_t     USB_MIDI_CABLE_NUMBER =         0;

    void init (void);
    void update (void);
    namespace MIDI {
        uint32_t available (void);
        void get (uint8_t *packet);
        void send (uint8_t msg[3]);
    }
}