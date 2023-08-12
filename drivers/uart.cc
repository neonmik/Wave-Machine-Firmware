#include "uart.h"

namespace UART {

    void init (void) {

        // unsigned char midichar = 0; // midi character rx'd and/or to be tx'd

        // set UART speed.
        uart_init(UART_ID, BAUD_RATE);

        // set UART Tx and Rx pins
        gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

        // enable Tx and Rx fifos on UART
        uart_set_fifo_enabled(UART_ID, true);

        // disable cr/lf conversion on Tx
        uart_set_translate_crlf(UART_ID, false);
    }
    void update (void) {

    }
    namespace MIDI {
        bool available (void) {
            return 0;
        }
        uint32_t buffer_size (void) {
            return 0;
        }
        void get (uint8_t *packet) {
            while (!uart_is_readable(UART_ID)){
            }
            uart_read_blocking(UART_ID, packet, 1);
        }
        void send (uint8_t msg[3]) {
            while (!uart_is_writable(UART_ID)){
            }
            uart_write_blocking(UART_ID, msg, 3);
            return;
        }
    }
}