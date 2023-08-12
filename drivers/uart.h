#pragma once

#include "pico/stdlib.h"

#define UART_ID uart1
#define BAUD_RATE 31250
#define UART_TX_PIN 4
#define UART_RX_PIN 5

namespace UART {
    void init (void);
    void update (void);
    namespace MIDI {
        bool available (void);
        uint32_t buffer_size (void);
        void get (uint8_t *packet);
        void send (uint8_t msg[3]);
    }
}