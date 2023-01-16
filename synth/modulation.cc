#include "modulation.h"

namespace modulation {

    void set_depth(int input) {
        depth = input;
    }

    void set_rate(int input) {
        rate = ((65536 * (input)) / env_rate)>>6;
    }

    void set_wave(int waveform) {
        wave = waveform*256;
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
        output = ((wavetable[wave + index]*depth)>>8); //
    }

    uint16_t get_output() {
        return (output+32768);
    }

    
}