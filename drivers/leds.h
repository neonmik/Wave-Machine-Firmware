#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
// #include "hardware/pwm.h"

#include "rgb.h"
#include "shiftreg.h"


enum Direction : uint8_t {
    UP              = 1,
    DOWN            = 0
};

enum Leds : uint8_t {
    LED_PICO_PIN    = PICO_DEFAULT_LED_PIN, // 25
    LED_LFO_PIN     = 21,
    LED_ARP_PIN     = 22,
};

namespace LEDS {
    
    class Led {
        private:
            uint8_t     _pin;
            bool        _state;

            void set (bool state) {
                gpio_put(_pin, state);
            }

        public:
            Led (uint8_t pin) {
                _pin = pin;
            }
            ~Led () { }

            void init(){
                gpio_init(_pin); // set LED pin
                gpio_set_dir(_pin, GPIO_OUT); // set LED pin to out
            }
            void on () {
                _state = 1;
                Led::set(_state);
            }
            void off (){
                _state = 0;
                Led::set(_state);
            }
            void toggle (void) {
                _state = !_state;
                Led::set(_state);
            }
            void flash (int repeats, int delay) {
                for (int r = 0; r < repeats; r++) {
                    toggle();
                    sleep_ms(delay);
                    toggle();
                    sleep_ms(delay);
                }
            }
        
    };
    class Rgb {

        uint16_t colours[24] = {
            65534, 0, 0,          // Red
            19018, 0, 46517,      // Purple
            65535, 0, 8995,       // Pink
            0, 29555, 38550,      // Teal
            2570, 0, 65534,       // Blue
            65534, 10023, 0,      // Orange
            3855, 65535, 0,       // Green
            65534, 25700, 23130   // Tutti Fruity
        };
        private:
            uint8_t _pins[3];
            uint16_t _colour[3];
            bool    _state;

        public:
            Rgb () { }
            ~Rgb () { }

            void init (void) {
                RGB_LED::init();
                colour(colours[0], colours[1], colours[2]);
                on();
            }
            void on (void) {
                _state = true;
                RGB_LED::on();
                RGB_LED::update();
            }
            void colour (uint16_t red, uint16_t green, uint16_t blue) {
                RGB_LED::set(red, green, blue);
                RGB_LED::update();
                
            }
            void off (void) {
                _state = false;
                RGB_LED::off();
                RGB_LED::update();
            }
            void toggle (void) {
                _state = !_state;
                if (_state) on();
                if (!_state) off();
            }
            void flash (int repeats, int delay) {
                for (int r = 0; r < repeats; r++) {
                    toggle();
                    sleep_ms(delay);
                    toggle();
                    sleep_ms(delay);
                }  
            }
            void cycle (int speed) {  
                // reset array to red incase wer'ee on a different colour
                _colour[0] = 65535;
                _colour[1] = 0;
                _colour[2] = 0;  

                // Choose the colours to increment and decrement.
                for (int decColour = 0; decColour < 3; decColour += 1) {
                    volatile int incColour = decColour == 2 ? 0 : decColour + 1;

                    // cross-fade the two colours.
                    for(int i = 0; i < 65535; i += 1) {
                    _colour[decColour] -= 1;
                    _colour[incColour] += 1;
                    
                    colour(_colour[0],_colour[1],_colour[2]);
                    sleep_us(speed);
                    }
                }
            }
            void preset (uint8_t preset) {
                uint8_t temp = (preset*3);
                colour(colours[temp], colours[temp+1], colours[temp+2]);
            }

    };
    class SR {
        private:
            Pins     _pin;
            bool     _state;

        public:
            SR (Pins pin){
                _pin = pin;
            }
            ~SR () { }

        void on () {
            _state = 1;
            ShiftReg::on_bit(_pin);
        }
        void off () {
            _state = 0;
            ShiftReg::off_bit(_pin);
        }
        void toggle () {
            _state = !_state;
            ShiftReg::toggle_bit(_pin);
        }
        void update () {
            ShiftReg::update();
        }
        void flash (int repeats, int delay){
            for (int r = 0; r < repeats; r++) {
                toggle();
                update();
                sleep_ms(delay);
                toggle();
                update();
                sleep_ms(delay);
            }
        }

    };
    
    
    extern Led PICO;
    extern Led LFO;
    extern Led ARP;
    extern Rgb PRESET;
    extern SR  KNOBS;
    extern SR  KNOB_1;
    extern SR  KNOB_2;
    extern SR  KNOB_3;
    extern SR  KNOB_4;
    extern SR  PAGES;
    extern SR  PAGE_1;
    extern SR  PAGE_2;
    extern SR  PAGE_3;
    extern SR  SPARE;



    void init(void);
    void on(void);
    void off(void);
    void KNOBS_off(void);
    void KNOB_select(uint8_t knob, bool state);
    void PAGES_off(void);
    void flash(int repeats, int delay);
    void test(uint8_t delay);
    void update(void);

}
