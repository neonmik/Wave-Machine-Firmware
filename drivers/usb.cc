#include "usb.h"

namespace USB {
    void init () {
        tusb_init();
    }

    void update () {
        tud_task(); // tinyusb device task
    }
    namespace MIDI {
        uint32_t available () {
            return tud_midi_n_available(0,0);
        }
        void get (uint8_t size, uint8_t *packet) {   
            tud_midi_n_stream_read(0, USB_MIDI_CABLE_NUMBER, packet, size);
        }
        void send (uint8_t *msg, uint8_t length) {
            if (!length) {
                DEBUG::error("USB MIDI Message has 0 length");
                return;
            }
            tud_midi_stream_write(USB_MIDI_CABLE_NUMBER, msg, length);
        }
    }
}

