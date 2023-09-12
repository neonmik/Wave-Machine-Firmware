#include "usb.h"

namespace USB {
    void Init () {
        tusb_init();
    }

    void Update () {
        tud_task(); // tinyusb device task
    }
    namespace MIDI {
        uint32_t available () {
            return tud_midi_n_available(0,0);
        }
        void get (uint8_t *packet) {   
            tud_midi_n_stream_read(0, USB_MIDI_CABLE_NUMBER, packet, 3);
        }
        void send (uint8_t msg[3]) {
            tud_midi_stream_write(USB_MIDI_CABLE_NUMBER, msg, 3);
        }
    }
}

