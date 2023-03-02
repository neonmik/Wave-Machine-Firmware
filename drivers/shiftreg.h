#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


#define SR_DATA_WIDTH   8
#define SR_DATA         18
#define SR_CLK          19
#define SR_LATCH        20

enum Pins : uint8_t {
        ALL     = 255,
        KNOBS   = 240,
        KNOB_1  = 128,
        KNOB_2  = 64,
        KNOB_3  = 32,
        KNOB_4  = 16,
        PAGES   = 14,
        PAGE_1  = 8,
        PAGE_2  = 4,
        PAGE_3  = 2,
        SPARE   = 1,
        OFF     = 0
};



namespace ShiftReg {
    

    class ShiftRegister {
        private:
            uint8_t     _data_pin;
            uint8_t     _clk_pin;
            uint8_t     _latch_pin;
            uint8_t     _data_width;
            uint8_t     _buffer;

            void step () {
                gpio_put(_clk_pin, 1);
                gpio_put(_clk_pin, 0);
            }
            void shift_out () {
                gpio_put(_latch_pin, 0);
                for (int i = 0; i < _data_width; i++)  {
                        gpio_put(_data_pin, !!(_buffer & (1 << i)));
                        step();
                }
                gpio_put(_latch_pin, 1);
            }

        public:
            ShiftRegister (uint8_t data_pin, uint8_t clk_pin, uint8_t latch_pin, uint8_t data_width) {
                _data_pin = data_pin;
                _clk_pin = clk_pin;
                _latch_pin = latch_pin;
                _data_width = data_width;
            }
            ~ShiftRegister () { }

            void init () {
                gpio_init(_data_pin);
                gpio_init(_clk_pin);
                gpio_init(_latch_pin);

                gpio_set_dir(_data_pin, GPIO_OUT);
                gpio_set_dir(_clk_pin, GPIO_OUT);
                gpio_set_dir(_latch_pin, GPIO_OUT);

                clear_buffer();
                shift_out();

            }
            void set_buffer (uint8_t buffer) {
                _buffer = buffer;
            }
            void clear_buffer () {
                _buffer = 0;
            }
            void update () {
                shift_out();
            }
    

    };
    
    
    void init();
    // ShiftReg::update(); call to shift out, but without any data handling, just taking the buffer and pushing it out the door...
    // can be used to update the shiftregister so many times a second (for fauxPWM or to reduce load on MCU), or whenever needed.
    void update (void);
    void set (Pins pins);
    void on (void);
    void on_bit (Pins pin);
    void toggle (void);
    void toggle_bit (Pins pin);
    void off (void);
    void off_bit (Pins pin);
    void clear (void);

}


// // Header file for ShiftRegPWM class

// #ifndef SHIFTREGPWM_H
// #define SHIFTREGPWM_H

// #include "pico/stdlib.h"

// class ShiftRegPWM {
// public:
// ShiftRegPWM(uint pin_sr_data, uint pin_sr_clk, uint pin_sr_latch, uint data_width, uint pwm_resolution);
// void set_pin(uint pin, bool value);
// void set_pin_pwm_duty_cycle(uint pin, uint duty_cycle);
// void set_pins_pwm_duty_cycle(uint8_t pins, uint duty_cycle);
// void set_all_pins_pwm_duty_cycles(uint duty_cycle);
// void set_pwm_resolution(uint resolution);
// void update();

// private:
// ShiftRegister _sr;
// uint _pwm_resolution;
// uint _pwm_time;
// uint8_t _buffer[8];
// uint8_t _pwm_duty_cycles[8];
// uint8_t _bit_mask[8];

// void _update_pwm_duty_cycles();
// };

// #endif // SHIFTREGPWM_H




    