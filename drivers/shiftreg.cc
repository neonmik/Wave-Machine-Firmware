#include "shiftreg.h"



namespace ShiftReg {
    
    // anonymous namespace - can only be used internally.
    namespace {
        uint8_t     _buffer[8];
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
        
        SR.set_buffer(_buffer[0]);
        SR.update();
        
        // tick();
        
        
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
        SR.clear_buffer();
        SR.update();
    }
}


// #include "pico/stdlib.h"
// #include "ShiftRegister.h"

// class ShiftRegPWM {
// public:
//     ShiftRegPWM(uint pin_sr_data, uint pin_sr_clk, uint pin_sr_latch, uint data_width, uint pwm_resolution);
//     void set_pin(uint pin, bool value);
//     void set_pin_pwm_duty_cycle(uint pin, uint duty_cycle);
//     void set_pins_pwm_duty_cycle(uint8_t pins, uint duty_cycle);
//     void set_all_pins_pwm_duty_cycles(uint duty_cycle);
//     void set_pwm_resolution(uint resolution);
//     void update();

// private:
//     ShiftRegister _sr;
//     uint _pwm_resolution;
//     uint _pwm_time;
//     uint8_t _buffer[8];
//     uint8_t _pwm_duty_cycles[8];
//     uint8_t _bit_mask[8];

//     void _update_pwm_duty_cycles();
// };

// ShiftRegPWM::ShiftRegPWM(uint pin_sr_data, uint pin_sr_clk, uint pin_sr_latch, uint data_width, uint pwm_resolution)
//     : _sr(pin_sr_data, pin_sr_clk, pin_sr_latch, data_width),
//       _pwm_resolution(pwm_resolution),
//       _pwm_time(0)
// {
//     // Initialize buffer and bit mask arrays to 0
//     for (int i = 0; i < 8; i++) {
//         _buffer[i] = 0;
//         _bit_mask[i] = 1 << i;
//         _pwm_duty_cycles[i] = 0;
//     }
// }

// void ShiftRegPWM::set_pin(uint pin, bool value) {
//     if (value) {
//         _buffer[0] |= _bit_mask[pin];
//     } else {
//         _buffer[0] &= ~_bit_mask[pin];
//     }
// }

// void ShiftRegPWM::set_pin_pwm_duty_cycle(uint pin, uint duty_cycle) {
//     _pwm_duty_cycles[pin] = duty_cycle;
// }

// void ShiftRegPWM::set_pins_pwm_duty_cycle(uint8_t pins, uint duty_cycle) {
//     for (int i = 0; i < 8; i++) {
//         if (pins & _bit_mask[i]) {
//             _pwm_duty_cycles[i] = duty_cycle;
//         }
//     }
// }

// void ShiftRegPWM::set_all_pins_pwm_duty_cycles(uint duty_cycle) {
//     for (int i = 0; i < 8; i++) {
//         _pwm_duty_cycles[i] = duty_cycle;
//     }
// }

// void ShiftRegPWM::set_pwm_resolution(uint resolution) {
//     _pwm_resolution = resolution;
// }

// void ShiftRegPWM::update() {
//     _update_pwm_duty_cycles();

//     _sr.set_buffer(_buffer[0]);
//     _sr.update();
// }

// void ShiftRegPWM::_update_pwm_duty_cycles() {
//     _pwm_time++;

//     if (_pwm_time >= _pwm_resolution) {
//         _pwm_time = 0;
//     }

//     for (int i = 0; i < 8; i++) {
//         if (_pwm_duty_cycles[i] == 0) {
//             // Pin is always off
//             _buffer[0] &= ~_bit_mask[i];
//         } else if (_pwm_duty_cycles[i] >= _pwm_time) {
//             // Pin is on for this duty cycle
//             _buffer[0] |= _bit_mask[i];
//         } else {
//             // Pin is off for this duty cycle
//             _buffer[0] &= ~_bit_mask[i];
//         }
//     }
// }