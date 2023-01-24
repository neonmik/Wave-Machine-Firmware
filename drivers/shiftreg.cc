#include "shiftreg.h"



namespace ShiftReg {
    
    // anonymous namespace - can only be used internally.
    namespace {
        uint8_t     _buffer;
        uint8_t     time            = 0;
        uint16_t    resolution      = 8;

        void tick (void) {
            time++;
            if (time == resolution) time = 0; 
        }
    }

    
    ShiftRegister SR(SR_DATA, SR_CLK, SR_LATCH, SR_DATA_WIDTH);
 

    void init() {
        SR.init();

    }
    void update() {

        SR.set_buffer(_buffer);
        SR.update();
        
        // tick();
        
    }
    void set (Pins pins) {
        uint8_t temp = static_cast<uint8_t>(pins);
        _buffer |= temp;
    }

    void on () {
        uint8_t temp = static_cast<uint8_t>(Pins::ALL);
        _buffer |= temp;
    }
    void on_bit (Pins pin) {
        uint8_t temp = static_cast<uint8_t>(pin);
        _buffer |= temp;
    }

    void toggle () {
        uint8_t temp = static_cast<uint8_t>(Pins::ALL);
        _buffer ^= temp;
    }
    void toggle_bit (Pins pin) {
        uint8_t temp = static_cast<uint8_t>(pin);
        _buffer ^= temp;
    }

    void off () {
        uint8_t temp = static_cast<uint8_t>(Pins::ALL);
        _buffer &= ~temp;
    }
    void off_bit (Pins pin) {
        uint8_t temp = static_cast<uint8_t>(pin);
        _buffer &= ~temp;
    }
    void clear () {
        SR.clear_buffer();
        SR.update();
    }
}