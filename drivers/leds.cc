#include "leds.h"

namespace LEDS {

    GPIO PICO(Leds::LED_PICO_PIN);
    GPIO FUNC1(Leds::LED_LFO_PIN);
    GPIO FUNC2(Leds::LED_ARP_PIN);
    RGB PRESET;
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
        FUNC1.init();
        FUNC2.init();
        PRESET.init();
        ShiftReg::init();
    }
    void on(){
        PICO.on();
        FUNC1.on();
        FUNC2.on();
        PRESET.on();
        _SR_state = true;
        ShiftReg::set_bit(Pins::ALL, _SR_state);
    }
    void off(){
        PICO.off();
        FUNC1.off();
        FUNC2.off();
        PRESET.off();
        _SR_state = false;
        ShiftReg::set_bit(Pins::ALL, _SR_state);
    }
    void toggle() {
        PICO.toggle();
        FUNC1.toggle();
        FUNC2.toggle();
        PRESET.toggle();
        _SR_state != _SR_state;
        ShiftReg::set_bit(Pins::ALL, _SR_state);
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
    void PAGE_select(uint8_t page) {
        PAGE_1.off();
        PAGE_2.off();
        PAGE_3.off();
        switch (page) {
            case (0):
                PAGE_1.on();
                break;
            case (1):
                PAGE_2.on();
                break;
            case (2):
                PAGE_3.on();
                break;
            case (3):
                PAGE_1.on();
                PAGE_2.on();
                PAGE_3.on();
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
        PICO.flashDelay(1, delay);
        KNOB_1.flashDelay(1, delay);
        KNOB_2.flashDelay(1, delay);
        KNOB_3.flashDelay(1, delay);
        KNOB_4.flashDelay(1, delay);
        PAGE_1.flashDelay(1, delay);
        PAGE_2.flashDelay(1, delay);
        PAGE_3.flashDelay(1, delay);
        FUNC1.flashDelay(1, delay);
        FUNC2.flashDelay(1, delay);

        PRESET.cycle(delay);

        // Down
        FUNC2.flashDelay(1, delay);
        FUNC1.flashDelay(1, delay);
        PAGE_3.flashDelay(1, delay);
        PAGE_2.flashDelay(1, delay);
        PAGE_1.flashDelay(1, delay);
        KNOB_4.flashDelay(1, delay);
        KNOB_3.flashDelay(1, delay);
        KNOB_2.flashDelay(1, delay);
        KNOB_1.flashDelay(1, delay);
        PICO.flashDelay(1, delay);
    }
    
    void update() {
        FUNC2.update();
        FUNC1.update();
        PRESET.update();
        // PAGE_1.update();
        
        ShiftReg::update();

        if (ADC::isBatteryLow()) {
            
        }
    }
}