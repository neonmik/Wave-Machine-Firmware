#pragma once

#include "pico/stdlib.h"

#include "bsp/board.h"
#include "tusb.h"

namespace USB {

    constexpr   uint8_t     USB_MIDI_CABLE_NUMBER =         0;

    void init (void);
    void update (void);
    namespace MIDI {
        bool available (void);
        uint32_t buffer_size (void);
        void get (uint8_t *packet);
        // void get_stream (void *buffer, uint32_t *length);
        void send (uint8_t msg[3]);
    }
}