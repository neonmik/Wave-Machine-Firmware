#include "uart.h"

namespace UART {

    void Init (void) {

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
        // if (uart_is_enabled(MIDI_UART_ID)) printf("UART ENABLED\n");

    }
    void Update (void) {

    }
    namespace MIDI {
        bool available (void) {
            return 0;
        }
        uint32_t buffer_size (void) {
            return 0;
        }
        bool get (uint8_t *packet) {
            // check there's data in the buffer, dont want to wait if theres no data to be read.
            if (uart_is_readable(MIDI_UART_ID)) {
                uart_read_blocking(MIDI_UART_ID, packet, 3);
                return true; // Data was successfully read
            }
            return false; // No data available to read
        }
        void send (uint8_t msg[3]) {
            uart_write_blocking(MIDI_UART_ID, msg, 3);
        }
    }
}