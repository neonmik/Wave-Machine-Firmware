#include "usb.h"

namespace USB {
    void init () {
        tusb_init();
    }

    void update () {
        tud_task(); // tinyusb device task
    }
    namespace MIDI {
        bool available () {
            return tud_midi_available();
        }
        uint32_t buffer_size () {
            return (tud_midi_n_available(0,0)/4);
        }
        void get (uint8_t *packet) {   
            tud_midi_packet_read(packet);
        }
        void send (uint8_t msg[3]) {
            tud_midi_stream_write(USB_MIDI_CABLE_NUMBER, msg, 3);
        }
    }
}

