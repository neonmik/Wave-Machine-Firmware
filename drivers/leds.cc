#include "leds.h"

namespace LEDS {

    Led PICO(Leds::LED_PICO_PIN);
    Led LFO(Leds::LED_LFO_PIN);
    Led ARP(Leds::LED_ARP_PIN);
    Rgb RGB;
    SR  KNOBS(Pins::KNOBS);
    SR  KNOB_1(Pins::KNOB_1);
    SR  KNOB_2(Pins::KNOB_2);
    SR  KNOB_3(Pins::KNOB_3);
    SR  KNOB_4(Pins::KNOB_4);
    SR  PAGES(Pins::PAGES);
    SR  PAGE_1(Pins::PAGE_1);
    SR  PAGE_2(Pins::PAGE_2);
    SR  PAGE_3(Pins::PAGE_3);
    SR  SPARE(Pins::SPARE);

    void init(void)
    {
        PICO.init();
        LFO.init();
        ARP.init();
        RGB.init();
        ShiftReg::init();

    }
    void on(){
        PICO.on();
        LFO.on();
        ARP.on();
        RGB.on();
        ShiftReg::on_bit(Pins::ALL);
    }
    void off(){
        PICO.off();
        LFO.off();
        ARP.off();
        RGB.off();
        ShiftReg::off_bit(Pins::ALL);
    }
    void toggle() {
        PICO.toggle();
        LFO.toggle();
        ARP.toggle();
        RGB.toggle();
        ShiftReg::toggle_bit(Pins::ALL);
    }
    
    void KNOBS_off(void) {
        KNOB_1.off();
        KNOB_2.off();
        KNOB_3.off();
        KNOB_4.off();
    }
    void KNOB_select(uint8_t knob, bool state) {
        switch (knob) {
            case (0):
                if (state) KNOB_1.on(); 
                if (!state) KNOB_1.off();
                break;
            case (1):
                if (state) KNOB_2.on(); 
                if (!state) KNOB_2.off();
                break;
            case (2):
                if (state) KNOB_3.on(); 
                if (!state) KNOB_3.off();
                break;
            case (3):
                if (state) KNOB_4.on(); 
                if (!state) KNOB_4.off();
                break;
        }
            
    }
    void PAGES_off(void) {
        PAGE_1.off();
        PAGE_2.off();
        PAGE_3.off();
    }
    
    void flash(int repeats, int delay) {
        for (int r = 0; r < repeats; r++) {
            toggle();
            update();
            sleep_ms(delay);
            toggle();
            update();
            sleep_ms(delay);
        }
    }
    
    void test(uint8_t delay){
        // flash(10, 100);
        // Up
        PICO.flash(1, delay);
        KNOB_1.flash(1, delay);
        KNOB_2.flash(1, delay);
        KNOB_3.flash(1, delay);
        KNOB_4.flash(1, delay);
        PAGE_1.flash(1, delay);
        PAGE_2.flash(1, delay);
        PAGE_3.flash(1, delay);
        LFO.flash(1, delay);
        ARP.flash(1, delay);

        RGB.cycle(delay/8);

        // Down
        ARP.flash(1, delay);
        LFO.flash(1, delay);
        PAGE_3.flash(1, delay);
        PAGE_2.flash(1, delay);
        PAGE_1.flash(1, delay);
        KNOB_4.flash(1, delay);
        KNOB_3.flash(1, delay);
        KNOB_2.flash(1, delay);
        KNOB_1.flash(1, delay);
        PICO.flash(1, delay);
    }
    
    void update() {
        ShiftReg::update();
        // RGB_LED::update();
    }
}