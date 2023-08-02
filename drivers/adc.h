#pragma once

#define MUX_SEL_A       12
#define MUX_SEL_B       13
#define MUX_SEL_C       14
#define MUX_SEL_D       15

#define MUX_OUT_ADC     26

#define MAX_KNOBS       4

#include "pico/stdlib.h"

#include "hardware/adc.h"
#include "hardware/dma.h"

#include "../config.h"
#include "../functions.h"

#include "../random.h"

namespace ADC {
    namespace {
        uint16_t _adc_value;
        uint8_t  _adc_noise;
        uint16_t _values[MAX_KNOBS];

        uint8_t _mux_address;
        uint32_t _sample[MAX_KNOBS];
        uint32_t _output;

        // change these to just handle the sample filter with a reference, and then add new function for saving to the table.
        inline void NO_filter (uint16_t reading, uint8_t index) {
            _sample[index] = (reading >> 2);
        }
        inline void IIR_filter (uint16_t reading, uint8_t index) {
            _sample[index] = _sample[index] - (_sample[index]>>2) + reading;
        }
        inline void FIR_filter (uint16_t reading, uint8_t index) {
            // nothing to report yet...
            // just do no filter for now and fire an error
            NO_filter(reading, index);
            printf("There is no FIR filter implemented yet! Saving as RAW data. Go check the ADC...");
        }
        inline void increment_mux_address (void) {
            // sets the index to loop
            _mux_address = (_mux_address + 1) % MAX_KNOBS;
        }
        void read_mux (void) {
            // sets mux pins
            gpio_put(MUX_SEL_A, _mux_address & 1); 
            gpio_put(MUX_SEL_B, (_mux_address >> 1) & 1);
            gpio_put(MUX_SEL_C, (_mux_address >> 2) & 1);
            gpio_put(MUX_SEL_D, (_mux_address >> 3) & 1);

            // wait to read - allows settling time
            asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
            
            _adc_value = adc_read(); 
            

            // zeros mux for keys
            gpio_put(MUX_SEL_A, 0);
            gpio_put(MUX_SEL_B, 0);
            gpio_put(MUX_SEL_C, 0);
            gpio_put(MUX_SEL_D, 0);
        }
    }
    
    void init();
    void update();
    uint16_t value(int knob);
    uint8_t noise();
}
