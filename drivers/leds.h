#pragma once

#include "../config.h"

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

    enum Speed : uint8_t {
        FAST            = 4,
        NORMAL          = 8, 
        SLOW            = 48, // probably should be used for constant flashing
    };
    namespace {
        bool _SR_state;
    }
    
    class GPIO {
        private:
            uint8_t     pin;
            bool        state;
            uint16_t    count;
            uint8_t     repeats;
            uint16_t    duration;
            bool        flashState;

            void put (bool input) {
                gpio_put(pin, input);
            }

        public:
            GPIO (uint8_t input) {
                pin = input;
            }
            ~GPIO () { }

            void init(){
                gpio_init(pin); // set LED pin
                gpio_set_dir(pin, GPIO_OUT); // set LED pin to out
                // off();
            }
            void on () {
                state = true;
                put(state);
            }
            void off (){
                state = false;
                put(state);
            }
            void toggle (void) {
                state = !state;
                put(state);
            }
            void set (bool input) {
                state = input;
                put(state);
            } 
            void flashDelay (int repeats, int delay){
                for (int r = 0; r < repeats; r++) {
                    toggle();
                    update();
                    sleep_ms(delay);
                    toggle();
                    update();
                    sleep_ms(delay);
                }
            }
            void flash (uint8_t number_repeats, Speed speed) {
                if (number_repeats == 0xFF) repeats = 0xFF;
                else repeats = number_repeats << 1; // number of times it repeats,  immediatly doubles to get the on and off

                duration = (speed << 3); // a speed setting of how fast the flash will be.

                flashState = !state; // sets opposite starting state so always flashes right.
            }
            void update (void) {
                if (!repeats) 
                    return;

                ++count; // Increments the flash counter. Being uint8_t should wrap at 256.

                if (count > duration) {
                    
                    put(flashState);
                    flashState = !flashState;
                    
                    count = 0;
                    
                    if (repeats != 0xFF) {
                        --repeats;
                        if (repeats == 0) {
                            duration = 0;
                            put(state);
                        }
                    }
                }
            }
        
    };
    class RGB {
        uint8_t colours[24] = {
            255,    0,      0,          // Red
            74,     0,      181,        // Purple
            255,    0,      35,         // Pink
            0,      115,    150,        // Teal
            10,     0,      255,        // Blue
            255,    59,     0,          // Orange
            15,     255,    0,          // Green
            255,    100,    90,         // Tutti Fruity
        };

        private:
            uint16_t _colour[3];
            bool    state;

            uint16_t    _count;
            uint8_t     _repeats;
            uint16_t    _duration;
            bool        flashState;

        public:
            RGB () { }
            ~RGB () { }

            void init (void) {
                RGB_LED::init();
                colour(colours[0], colours[1], colours[2]);
                off();
            }
            void on (void) {
                state = true;
                RGB_LED::on();
                RGB_LED::update();
            }
            void colour (uint16_t red, uint16_t green, uint16_t blue) {
                if (red || green || blue) state = true; // as long as somehtings on a bit, state is on.
                else state = false;
                RGB_LED::set(red, green, blue);
                RGB_LED::update();
            }
            void off (void) {
                state = false;
                RGB_LED::off();
                RGB_LED::update();
            }
            void toggle (void) {
                state = !state;
                if (state) on();
                if (!state) off();
            }
            void set (bool input) {
                state = input;
                if (state) on();
                if (!state) off();
            }
            void reset (void) {
                if (state) on();
                if (!state) off();
            }
            void preset (uint8_t preset) {
                uint8_t temp = (preset*3);
                colour(colours[temp], colours[temp+1], colours[temp+2]);
            }
            void cycle (uint16_t speed) {  
                // reset array to red incase we're on a different colour
                _colour[0] = PWM_BIT_DEPTH;
                _colour[1] = 0;
                _colour[2] = 0;

                if (!speed) speed = 1; 
                speed <<= 4;

                // Choose the colours to increment and decrement.
                for (int decColour = 0; decColour < 3; decColour += 1) {
                    volatile int incColour = decColour == 2 ? 0 : decColour + 1;

                    // cross-fade the two colours.
                    for(int i = 0; i < PWM_BIT_DEPTH; i += 1) {
                        _colour[decColour] -= 1;
                        _colour[incColour] += 1;
                        
                        colour(_colour[0],_colour[1],_colour[2]);
                        sleep_us(speed);
                    }
                }
            }
            void flashDelay (uint8_t repeats, uint16_t delay){
                for (int r = 0; r < repeats; r++) {
                    toggle();
                    update();
                    sleep_ms(delay);
                    toggle();
                    update();
                    sleep_ms(delay);
                }
            }
            void flash (uint8_t repeats, uint8_t duration) {
                if (repeats == 0xFF) repeats = 0xFF;
                else _repeats = repeats << 1; // number of times it repeats,  immediatly doubles to get the on and off

                _duration = (duration << 3); // a speed setting of how fast the flash will be.

                flashState = !state; // sets opposite starting state so always flashes right.
            }
            void update (void) {
                if (!_repeats) return;

                ++_count;

                if (_count > _duration) {
                    
                    if (flashState) RGB_LED::on();
                    else RGB_LED::off();
                    RGB_LED::update();
                    flashState = !flashState;
                    
                    _count = 0;
                    
                    if (_repeats != 0xFF) {
                        --_repeats;
                        if (_repeats == 0) {
                            _duration = 0;
                            reset(); // reset original state.
                        }
                    }
                }
            }

    };
    class SR {
        private:
            Pins     _pin;
            bool     state;

            uint16_t    _count;
            uint8_t     _repeats;
            uint16_t     _duration;
            bool        _flash_state;

        public:
            SR (Pins pin){
                _pin = pin;
            }
            ~SR (void) { }

        void on (void) {
            state = true;
            ShiftReg::set_bit(_pin, state);
        }
        void off (void) {
            state = false;
            ShiftReg::set_bit(_pin, state);
        }
        void toggle (void) {
            state = !state;
            ShiftReg::set_bit(_pin, state);
        }
        void reset (void) {
            ShiftReg::set_bit(_pin, state);
        }
        void flashDelay (int repeats, int delay){
            for (int r = 0; r < repeats; r++) {
                toggle();
                ShiftReg::update();
                sleep_ms(delay);
                toggle();
                ShiftReg::update();
                sleep_ms(delay);
            }
        }
        void flash (uint8_t repeats, uint8_t duration) {
            if(repeats == 0xFF) _repeats = 0xFF;
            else _repeats = repeats << 1; // number of times it repeats

            _duration = (duration << 3); // 0 being no duration, 255 being always on.

            _flash_state = !state; // sets opposite starting state so always flashes right. 
        }

        bool update (void) {    
            if (!_repeats) return false;

            ++_count; // Increments the flash counter. Being uint8_t should wrap at 256.

            if (_count > _duration) {
                
                ShiftReg::set_bit(_pin, _flash_state);
                _flash_state = !_flash_state;
                
                _count = 0;
                
                if (_repeats != 0xFF) {
                    --_repeats;
                    if (_repeats == 0) {
                        _duration = 0;
                        reset(); // reset original state.
                        return false;
                    }
                }
            }

            return true;
        }
    };
    
    
    extern GPIO PICO;
    extern GPIO FUNC1;
    extern GPIO FUNC2;
    extern RGB PRESET;
    extern SR  KNOBS; // for clearing only!
    extern SR  KNOB_1;
    extern SR  KNOB_2;
    extern SR  KNOB_3;
    extern SR  KNOB_4;
    extern SR  PAGES; // for clearing only!
    extern SR  PAGE_1;
    extern SR  PAGE_2;
    extern SR  PAGE_3;
    extern SR  SPARE;



    void init(void);
    void on(void);
    void off(void);
    void KNOBS_off(void);
    void KNOB_select(uint8_t knob, bool input);
    void PAGES_off(void);
    void PAGE_select(uint8_t page);
    void flash(int repeats, int delay);
    void test(uint8_t delay);
    void update(void);

}
