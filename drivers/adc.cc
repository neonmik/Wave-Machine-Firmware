#include "adc.h"

namespace ADC {
    void init() {

        adc_init();

        read_onboard_temperature();

        adc_mux_init();

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

        gpio_set_drive_strength(MUX_SEL_A, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(MUX_SEL_B, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(MUX_SEL_C, GPIO_DRIVE_STRENGTH_2MA);
        gpio_set_drive_strength(MUX_SEL_D, GPIO_DRIVE_STRENGTH_2MA);

        // adc pin setup

        _mux_address = 0;

        // run the ADC 4 times to make sure the values are primed
        for (int i = 0; i > MAX_KNOBS; i++) {
            update();
        }
    }
    void update() {
        
        read_mux();
        
        // No filter
        // passes last actual sample to a buffer
        // NO_filter(_adc_value, _mux_address);
        
        // IIR filter
        // filters new sample with old sample
        IIR_filter(_adc_value, _mux_address);

        // FIR filter - to Be Imlpemented
        // FIR_filter(_adc_value, _mux_address);
        
        // moves filtered sample to the adc array
        _values[_mux_address] = map_constrained(_sample[_mux_address]>>2, 10, 4095, KNOB_MIN, KNOB_MAX);

        // Improve this to have more random numbers, make the RNG better.
        _adc_noise = _adc_value & 0x03; // bit mask the lower 2 bits to use as a natural noise source

        // RANDOM::update(_adc_noise);

        increment_mux_address();
    }
    
    uint16_t value(int knob) {
        return _values[knob]; // downshifting for better system wide performance
    }
    uint8_t noise() {
        return _adc_noise;
    }
    float temp (void) {
        return _core_temp;
    }
}