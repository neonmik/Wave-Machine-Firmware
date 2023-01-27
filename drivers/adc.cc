#include "adc.h"

namespace ADC {
    void init() {
        gpio_init(MUX_SEL_A);
        gpio_init(MUX_SEL_B);
        gpio_init(MUX_SEL_C);
        gpio_init(MUX_SEL_D);
        // set the pins direction
        gpio_set_dir(MUX_SEL_A, GPIO_OUT);
        gpio_set_dir(MUX_SEL_B, GPIO_OUT);
        gpio_set_dir(MUX_SEL_C, GPIO_OUT);
        gpio_set_dir(MUX_SEL_D, GPIO_OUT);
        // set the slew rate slow (for reducing amount of cross talk on address changes... hopefully)
        gpio_set_slew_rate(MUX_SEL_A, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(MUX_SEL_B, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(MUX_SEL_C, GPIO_SLEW_RATE_SLOW);
        gpio_set_slew_rate(MUX_SEL_D, GPIO_SLEW_RATE_SLOW);

        // adc pin setup
        adc_gpio_init(MUX_OUT_ADC);
        adc_init();
        adc_select_input(0);

        _mux_address = 0;
    }
    void update() {
        gpio_put(23, 1); // sets SMPS into low power mode for better reading on the ADC - need to validate... 

        // sets mux pins
        gpio_put(MUX_SEL_A, _mux_address & 1); 
        gpio_put(MUX_SEL_B, (_mux_address >> 1) & 1);
        gpio_put(MUX_SEL_C, (_mux_address >> 2) & 1);
        gpio_put(MUX_SEL_D, (_mux_address >> 3) & 1);

        // wait to read - allows settling time
        asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
        
        // IIR filter
        // passes last actual sample to a buffer
        // _sample[_mux_address] = _values[_mux_address];
        // filters new sample with old sample
        _sample[_mux_address] = _sample[_mux_address] - (_sample[_mux_address]>>2) + adc_read();

        // moves filtered sample to the adc array
        _values[_mux_address] = (_sample[_mux_address]>>2);  

        // sets the index to loop
        _mux_address = (_mux_address + 1) % MAX_KNOBS;

        // puts SMPS back into PWM mode
        gpio_put(23, 0); 

        // zeros mux for keys
        gpio_put(MUX_SEL_A, 0);
        gpio_put(MUX_SEL_B, 0);
        gpio_put(MUX_SEL_C, 0);
        gpio_put(MUX_SEL_D, 0);
    }
    
    uint16_t value(int knob) {
        return (_values[knob]>>2); // downshifting for better system wide performance
    }
}