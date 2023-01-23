#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"


enum Pins : uint8_t {
    LED_PICO_PIN    = 25,
    LED_LFO_PIN     = 21,
    LED_ARP_PIN     = 22,
    LEDR_PIN        = 6,
    LEDG_PIN        = 7,
    LEDB_PIN        = 8
};



namespace Leds {

    class Led {
        private:
            uint8_t     _pin;
            bool        _state;
        
            

        public:

            Led (uint8_t pin) {
                _pin = pin;
            }
            ~Led () { }

            void init ();
            void set (bool state);
            void on (void);
            void off (void);
            void toggle (void);
            void flash (int repeats, int delay);
        
    };

    class Rgb {
        private:
            uint8_t     _pin[3];
            uint16_t    _colour[3];
            bool        _state;

            

            void pwm_pin_init (int pin);
            void pwm_output_polarity (void);
        
        public:

            Rgb (uint8_t red_pin, uint8_t green_pin, uint8_t blue_pin) {
                _pin[0] = red_pin;
                _pin[1] = green_pin;
                _pin[2] = blue_pin;
            }
            ~Rgb () { }

            void init ();
            void set (uint16_t red, uint16_t green, uint16_t blue);
            void on (void);
            void off(void);
            void colour(uint16_t red, uint16_t green, uint16_t blue);
            void colour_8bit(uint8_t red, uint8_t green, uint8_t blue);
            void update (void);
            void recall (void);
            void clear (void);
            void toggle (void);
            void cycle (int speed);
            void flash (int repeats, int delay);
            void preset (uint8_t preset);
        
    };

    extern Led PICO;
    extern Led LFO;
    extern Led ARP;
    extern Rgb RGB;

    void init(void);
    void on(void);
    void off(void);
    void test(uint8_t delay);

}

