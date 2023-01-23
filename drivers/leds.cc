#include "leds.h"

namespace Leds {

    void Led::init(){
        gpio_init(_pin); // set LED pin
        gpio_set_dir(_pin, GPIO_OUT); // set LED pin to out
    }
    void Led::on () {
        _state = 1;
        Led::set(_state);
    }
    void Led::off (){
        _state = 0;
        Led::set(_state);
    }
    void Led::set (bool state) {
        gpio_put(_pin, state);
    }
    void Led::toggle (void) {
        _state = !_state;
        Led::set(_state);
    }
    void Led::flash (int repeats, int delay) {
        for (int r = 0; r < repeats; r++) {
            Led::toggle();
            sleep_ms(delay);
            Led::toggle();
            sleep_ms(delay);
        }
    }

    void Rgb::init() {
        pwm_pin_init(_pin[0]);
        pwm_pin_init(_pin[1]);
        pwm_pin_init(_pin[2]);

        pwm_output_polarity ();
        
        Rgb::preset(0); // set the default position for the RGB LED
    }
    void Rgb::on () {
        _state = true;
        Rgb::set(0xFFFF, 0xFFFF, 0xFFFF);
    }
    void Rgb::set (uint16_t red, uint16_t green, uint16_t blue) {
        pwm_set_gpio_level(_pin[0], red);
        pwm_set_gpio_level(_pin[1], green);
        pwm_set_gpio_level(_pin[2], blue);
    }
    void Rgb::update () {
        pwm_set_gpio_level(_pin[0], _colour[0]);
        pwm_set_gpio_level(_pin[1], _colour[1]);
        pwm_set_gpio_level(_pin[2], _colour[2]);
    }
    void Rgb::off (){
        _state = false;
        Rgb::set(0, 0, 0);
    }
    void Rgb::colour (uint16_t red, uint16_t green, uint16_t blue) {
        _colour[0] = red;
        _colour[1] = green;
        _colour[2] = blue;
    }
    void Rgb::colour_8bit (uint8_t red, uint8_t green, uint8_t blue) {
        _colour[0] = (red<<8);
        _colour[1] = (green<<8);
        _colour[2] = (blue<<8);
    }
    void Rgb::clear (void) {
        _colour[0] = 0;
        _colour[1] = 0;
        _colour[2] = 0;
    }
    void Rgb::recall (void) {
        Rgb::set(_colour[0], _colour[1], _colour[2]);
    }
    void Rgb::toggle (void) {
        // may have to swap this for an if-else statment, or swap the ! for a ~
        _state = !_state;
        Rgb::set(!_colour[0], !_colour[1], !_colour[2]);
    }
    void Rgb::cycle (int speed) {  
        // reset array to blue incase wer'e on a different colour
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
            
            set(_colour[0],_colour[1],_colour[2]);
            update();
            sleep_us(speed);
            }
        }
    }
    void Rgb::flash (int repeats, int delay) {
        for (int r = 0; r < repeats; r++) {
            Rgb::toggle();
            sleep_ms(delay);
            Rgb::toggle();
            sleep_ms(delay);
        }
    }
    void Rgb::preset (uint8_t preset) {
        switch (preset) {
            case 0: //red
                Rgb::colour_8bit(255, 0, 0);
                Rgb::update();
                break;
            case 1: // purple
                Rgb::colour_8bit(74, 0, 181);
                Rgb::update();
                break;
            case 2: // pink
                Rgb::colour_8bit(255, 0, 35);
                Rgb::update();
                break;
            case 3: // teal
                Rgb::colour_8bit(0, 115, 150);
                Rgb::update();
                break;
            case 4: // blue
                Rgb::colour_8bit(10, 0, 255);
                Rgb::update();
                break;
            case 5: // orange
                Rgb::colour_8bit(255, 39, 0);
                Rgb::update();
                break;
            case 6: // green
                Rgb::colour_8bit(15, 255, 0);
                Rgb::update();
                break;
            case 7: // tutti frutti
                Rgb::colour_8bit(255, 100, 90);
                Rgb::update();
                break;
        } 
    }
    void Rgb::pwm_pin_init (int pin) {
        gpio_set_function(pin, GPIO_FUNC_PWM);
        uint slice_num = pwm_gpio_to_slice_num(pin);
        
        pwm_set_gpio_level(pin, 0);
        pwm_set_enabled(slice_num, true);
    }
    void Rgb::pwm_output_polarity (void) {
        uint slice_num0 = pwm_gpio_to_slice_num(_pin[0]);
        uint slice_num1 = pwm_gpio_to_slice_num(_pin[2]);
        pwm_set_output_polarity(slice_num0, true, true);
        pwm_set_output_polarity(slice_num1, true, false);
    }


    Led PICO(Pins::LED_PICO_PIN);
    Led LFO(Pins::LED_LFO_PIN);
    Led ARP(Pins::LED_ARP_PIN);
    Rgb RGB(Pins::LEDR_PIN, Pins::LEDG_PIN, Pins::LEDB_PIN);


    void init(void)
    {

        PICO.init();

        LFO.init();
        ARP.init();

        RGB.init();
    }
    void on(){
        PICO.on();
        LFO.on();
        ARP.on();
        RGB.on();
    }
    void off(){
        PICO.off();
        LFO.off();
        ARP.off();
        RGB.off();
    }
    void test(uint8_t delay){
        PICO.flash(1, delay);
        // put shift reg here
        LFO.flash(1, delay);
        ARP.flash(1, delay);
        RGB.cycle(delay/4);
        ARP.flash(1, delay);
        LFO.flash(1, delay);
        // put shift reg here
        PICO.flash(1, delay);
    }
}