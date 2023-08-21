#include "shiftreg.h"

#include "sn74595.pio.h"

namespace ShiftReg {
    void Init() {
        gpio_set_drive_strength(SRPins::SR_CLK, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(SRPins::SR_DATA, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(SRPins::SR_LATCH, GPIO_DRIVE_STRENGTH_2MA);

        gpio_set_slew_rate(SRPins::SR_CLK, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(SRPins::SR_DATA, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(SRPins::SR_LATCH, GPIO_SLEW_RATE_SLOW);

        sn74595::shiftreg_init();
    }
    void Update() {
        
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
        Update();
    }
}