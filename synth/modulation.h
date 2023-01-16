#pragma once

#include "pico/stdlib.h"


#include "wavetable.h"

// This wants to be a LFO for modulating different sources...
// I think it should be between 1Hz-20Hz (Or ???100Hz???)
// Could be used for:
//                      - Vibrato (Note frequency modulations)
//                      - Tremelo (Note Volume Modulations)
//                      - Wave Vector Modulations
//                      - A Filter?
namespace modulation {

    enum Matrix {
        OFF = 0,
        VIBRATO = 1,
        TREM = 2,
        VECTOR = 3
    };

    static uint8_t    index       = 0;
    static uint32_t    increment   = 0;
    static uint16_t    acc         = 0;
    static int16_t     output      = 0;

    static uint16_t    wave        = 0;
    static uint16_t    depth       = 0;
    volatile static uint16_t    rate        = 0;

    static uint8_t     matrix      = Matrix::OFF;

    static uint8_t     lfo_rate    = 255; //figure this out


    void set_depth(int input);
    void set_rate(int input);
    void set_wave(int waveform);
    void set_matrix(uint8_t input);
    uint8_t get_matrix(void);

    void init (void);
    void update (void);
    uint16_t get_output_uint(void);
    int16_t get_output_int(void);
    


}

