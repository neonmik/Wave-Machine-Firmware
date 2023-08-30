#pragma once

#include "pico/stdlib.h"
#include "stdio.h"

#define MIDI_UART_ID uart1
#define MIDI_BAUD_RATE 31250
#define MIDI_UART_TX_PIN 4
#define MIDI_UART_RX_PIN 5

namespace UART {
    void Init (void);
    void Update (void);
    namespace MIDI {
        bool available (void);
        bool get (uint8_t *packet);
        void send (uint8_t msg[3]);
    }
}