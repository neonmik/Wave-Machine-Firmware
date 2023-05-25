#include "shiftreg.h"

#include "sn74595.pio.h"

namespace ShiftReg {
    void init() {
        sn74595::shiftreg_init();
    }
    void update() {
        
        for (int i = 0; i < 8; i++) {
            sn74595::setOutput(0, i, (_buffer[0] & (1 << i)));
        }
        sn74595::sendOutput();
        // tick(); // for updating the faux_PWM yet to be implemented
    }

    void set (Pins pins) {
        uint8_t temp = static_cast<uint8_t>(pins);
        _buffer[0] |= temp;
    }
    void on () {
        uint8_t temp = static_cast<uint8_t>(Pins::ALL);
        _buffer[0] |= temp;
    }
    void on_bit (Pins pin) {
        uint8_t temp = static_cast<uint8_t>(pin);
        _buffer[0] |= temp;
    }

    void toggle () {
        uint8_t temp = static_cast<uint8_t>(Pins::ALL);
        _buffer[0] ^= temp;
    }
    void toggle_bit (Pins pin) {
        uint8_t temp = static_cast<uint8_t>(pin);
        _buffer[0] ^= temp;
    }

    void off () {
        uint8_t temp = static_cast<uint8_t>(Pins::ALL);
        _buffer[0] &= ~temp;
    }
    void off_bit (Pins pin) {
        uint8_t temp = static_cast<uint8_t>(pin);
        _buffer[0] &= ~temp;
    }
    void clear () {
        off();
        update();
    }
}