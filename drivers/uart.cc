#include "uart.h"

namespace UART {

    void init (void) {

        // unsigned char midichar = 0; // midi character rx'd and/or to be tx'd

        // set UART speed.
        uart_init(MIDI_UART_ID, MIDI_BAUD_RATE);

        // set UART Tx and Rx pins
        gpio_set_function(MIDI_UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(MIDI_UART_RX_PIN, GPIO_FUNC_UART);

        // enable Tx and Rx fifos on UART
        uart_set_fifo_enabled(MIDI_UART_ID, true);

        // disable cr/lf conversion on Tx
        uart_set_translate_crlf(MIDI_UART_ID, false);
    }
    void update (void) {

    }
    namespace MIDI {
        bool available (void) {
            return uart_is_readable(MIDI_UART_ID);
        }
        uint32_t buffer_size (void) {
            return 0;
        }
        uint8_t get (void) {
            return uart_getc(MIDI_UART_ID);
        }
        void send (uint8_t *msg, uint8_t length) {
            if (!length) {
                DEBUG::error("UART MIDI Message has 0 length");
                return;
            }
            uart_write_blocking(MIDI_UART_ID, msg, length);
        }
    }
}