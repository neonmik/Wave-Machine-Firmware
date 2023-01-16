#include "modulation.h"

#include <math.h>

namespace modulation {

    void set_depth(int input) {
        // depth = log(input+1)/log(1024)*255;
        depth = (input>>2);
    }

    void set_rate(int input) {
        rate = ((65536 * (input)) / lfo_rate)>>6;
    }

    void set_wave(int waveform) {
        wave = (waveform>>6)*256;
    }

    void set_matrix(uint8_t input) {
        matrix = input;
    }
    uint8_t get_matrix() {
        return matrix;
    }

    void init (void) {
        set_depth(0);
        set_rate(0);
        set_wave(0);
    }

    void update () {
        // output = 0;
        acc += rate;
        index = acc >> 8;
        output = ((wavetable[wave + index]*depth)>>10); // the bit shift is to compenstate for the huge jump when multiplying the depth
    }

    uint16_t get_output_uint() {
        return (output+32767);
    }
    int16_t get_output_int() {
        return (output);
    }
    
}