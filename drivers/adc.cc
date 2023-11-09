#include "adc.h"

namespace ADC {
    void init() {

        adc_init();

        // Before setting up the pots, do a quick read of the startup temp
        read_onboard_temperature();

        // Setup the mux pins
        pin_init(MUX_SEL_A);
        pin_init(MUX_SEL_B);
        pin_init(MUX_SEL_C);
        pin_init(MUX_SEL_D);
        
        adc_mux_init();

        _mux_address = 0;

        // sleep_ms(100);
        
        // run the ADC 4 times to make sure the values are primed - removed from loop as it was optimised out
        update();
        update();
        update();
        update();
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