#include "shiftreg.h"

#include "sn74595.pio.h"

namespace ShiftReg {
    void init() {
        gpio_set_drive_strength(SRPins::SR_CLK, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(SRPins::SR_DATA, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(SRPins::SR_LATCH, GPIO_DRIVE_STRENGTH_2MA);

        gpio_set_slew_rate(SRPins::SR_CLK, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(SRPins::SR_DATA, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(SRPins::SR_LATCH, GPIO_SLEW_RATE_SLOW);

        sn74595::shiftreg_init();
    }
    void update() {
        if (!needsSending) return;
        
        for (int i = 0; i < 8; i++) {
            sn74595::setOutput(0, i, (_buffer[0] & (1 << i)));
        }
        sn74595::sendOutput();

        needsSending = false;
    }

    void set_bit(Pins pin, bool value) {
        uint8_t temp = static_cast<uint8_t>(pin);
        if (value) {
            _buffer[0] |= temp;
        } else {
            _buffer[0] &= ~temp;
        }
        needsSending = true;
    }

    void off () {
        uint8_t temp = static_cast<uint8_t>(Pins::ALL);
        _buffer[0] &= ~temp;
        needsSending = true;
    }

    void clear () {
        off();
        update();
    }
}