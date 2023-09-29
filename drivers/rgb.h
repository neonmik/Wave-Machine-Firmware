#pragma once

#include "../config.h"

#include "hardware/pwm.h"

#define    LEDR_PIN        6
#define    LEDG_PIN        7
#define    LEDB_PIN        8

#define    PWM_BIT_DEPTH   255 // or 65535


namespace RGB_LED {
    
    class rgb {
        private:
            uint8_t     _pin[3];
            uint16_t    _buffer[3];
            bool        _state;

            void pwm_pin_init (int pin) {
                uint slice_num = pwm_gpio_to_slice_num(pin);

                gpio_set_function(pin, GPIO_FUNC_PWM);
                gpio_set_slew_rate(pin, gpio_slew_rate::GPIO_SLEW_RATE_SLOW);
                gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_2MA);
                
                pwm_set_gpio_level(pin, 0);
                pwm_set_wrap(slice_num, PWM_BIT_DEPTH);
                pwm_set_clkdiv(slice_num, 256.0f); // should be about 2.2KHz?...  144MHz (core speed) / 256 (wrap) / 256 (clkdiv) = 2197.27
                pwm_set_enabled(slice_num, true);
            }
            void pwm_output_polarity (void) {
                uint slice_num0 = pwm_gpio_to_slice_num(_pin[0]);
                uint slice_num1 = pwm_gpio_to_slice_num(_pin[2]);
                pwm_set_output_polarity(slice_num0, true, true);
                pwm_set_output_polarity(slice_num1, true, false);
            }
            void set () {
                pwm_set_gpio_level(_pin[0], _buffer[0]);
                pwm_set_gpio_level(_pin[1], _buffer[1]);
                pwm_set_gpio_level(_pin[2], _buffer[2]);
            }
        
        public:
            rgb (uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin) {
                _pin[0] = red_pin;
                _pin[1] = green_pin;
                _pin[2] = blue_pin;
            }
            ~rgb () { }

            void init (void) {


                pwm_pin_init(_pin[0]);
                pwm_pin_init(_pin[1]);
                pwm_pin_init(_pin[2]);

                pwm_output_polarity();


                
                clear_buffer();
                set();
            }
            void set_buffer(uint16_t red, uint16_t green, uint16_t blue) {
                _buffer[0] = red;
                _buffer[1] = green;
                _buffer[2] = blue;
            }
            void clear_buffer (void) {
                _buffer[0] = 0;
                _buffer[1] = 0;
                _buffer[2] = 0;
            }
            void update (void) {
                set();
            }
        
    };

    extern rgb Rgb;

    void init (void);
    void on (void);
    void update (void);
    void set (uint16_t red, uint16_t green, uint16_t blue);
    void off (void);
    void clear (void);


}