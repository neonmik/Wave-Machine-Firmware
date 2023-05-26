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
    }
    
    void init();
    void update();
    uint16_t value(int knob);
    uint8_t noise();
}
