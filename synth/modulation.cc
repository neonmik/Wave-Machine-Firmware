#include "modulation.h"

#include <math.h>

extern uint16_t   synth::tremelo;



namespace modulation {



    void set_depth(int input) {
        // depth = log(input+1)/log(1024)*255; // didn't work, trying to create and audio taper
        if (matrix == Matrix::VIBRATO) depth = (input>>6); //not right
        if (matrix == Matrix::TREM) depth = (input); // PERFECT
        if (matrix == Matrix::VECTOR) depth = (input>>4); // Not right
        else depth = (input);
    }

    void set_rate(int input) {
        rate = ((65536 * (input)) / lfo_rate)>>6;
    }

    void set_wave(int waveform) {
        if ((waveform>>6) != last_wave) {
            wave = (waveform>>6)*256; // downshifted to give 0-15, then multipled to give the start of wavetable locations
            last_wave = waveform>>6;
        }
    }

    void set_matrix(uint8_t input) {
        if (input != last_matrix) {
            matrix = input;
            last_wave = input;
            init();
        }
    }

    uint8_t get_matrix() {
        return matrix;
    }

    void init (void) {
        set_depth(0);
        set_rate(0);
        set_wave(0);
    }

    void start(void) {
        active_ = true;
    }
    void toggle(void){
        active_ = !active_;
    }
    void stop(void) {
        active_ = false;
    }

    void update () {
        if (active_) {
            acc += rate;
            index = acc >> 8;
            output = ((wavetable[wave + index]*depth)>>10); // the bit shift is to compenstate for the huge jump when multiplying the depth
            if (matrix == Matrix::OFF) {
                synth::vibrato = 0;
                synth::tremelo = 0;
                synth::vector_mod = 0;
            }
            if (matrix == Matrix::VIBRATO) {
                synth::vibrato = (output>>10); // ideal -511~ - +511~, maybe -255 - +255
                synth::tremelo = 0;
                synth::vector_mod = 0;
            }
            if (matrix == Matrix::TREM) {
                synth::tremelo = uint16_t(int32_t(output)+32767); // ideal 0 - 65534
                synth::vibrato = 0;
                synth::vector_mod = 0;
            }
            if (matrix == Matrix::VECTOR) {
                synth::vector_mod = (uint16_t(int32_t(output)+32767))>>4; // ideal is 0 - 255  
                synth::tremelo = 0;
                synth::vibrato = 0;
            }
        }

        // if (!active_) {
        //     if (synth::vibrato || synth::tremelo || synth::vector_mod) {
                
        //     }
        // }
        
    }

    void clear () {
        acc     = 0;
        index   = 0;
        output  = 0;
        depth   = 0;
        rate    = 0;
        wave    = 0;
    }
    

    // old functions to be deprecated
    uint16_t get_output_uint() {
        return (int32_t(output)+32767);
    }
    int16_t get_output_int() {
        return (output);
    }

}