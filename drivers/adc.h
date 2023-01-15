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


class Adc {
    private:
        uint16_t adc_value_;
        uint16_t values_[MAX_KNOBS];

        uint8_t mux_address_;
        uint32_t sample_[MAX_KNOBS];
        uint32_t output_;

    public:
        Adc() { }
        ~Adc() { }

        void init();
        void update();

        inline int16_t value(int knob) const {
            return values_[knob];
        }


};
