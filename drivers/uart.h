#pragma once

#include "../config.h"

#define MIDI_UART_ID uart1
#define MIDI_BAUD_RATE 31250
#define MIDI_UART_TX_PIN 4
#define MIDI_UART_RX_PIN 5

namespace UART {
    void init (void);
    void update (void);
    namespace MIDI {
        bool available (void);
        uint8_t get (void);
        void send (uint8_t *msg, uint8_t length);
    }
}