#pragma once

#include "pico/stdlib.h"

#include "../drivers/adc.h"

#include "synth.h"
#include "arp.h"

#include "wavetable.h"
// Wave table size
#define WAVE_TABLE_SIZE 256

uint8_t ADC::noise();

namespace MOD {

    enum Dither : uint8_t {
        FULL,
        HALF,
        LOW,
        OFF
    };

    enum OutputTypes : uint8_t {
        UNSIGNED_INT_10,
        SIGNED_INT_8,
        NOT_ASSIGNED
    };

    struct OutputParams {
        void            (*variable)(uint16_t);
        // OutputTypes     type;
        uint8_t         offset;
        Dither          dither;
        volatile uint16_t        output = 0;
    };


    class Modulation {
        private:
            // main info
            uint32_t _sample_rate;

            // oscillator variables
        
            uint32_t _increment;
            uint32_t _phase_accumulator;
            uint16_t _phase_fractional;
            uint8_t _index;
            int16_t  _sample;

            uint32_t _frequency;
            const double RATE_SCALE_FACTOR = 0.01; // Define the scaling factor to convert 10-bit value to rate in Hz
            
            // control variables
            bool     _state = false;
            uint8_t  _matrix;
            uint16_t _rate;
            uint16_t _depth;
            uint16_t  _wave;

            uint16_t prng_seed = 0;
            
            OutputParams _destination[4]{
                // pointer of what to update, type of output, offset for output table
                {&SYNTH::modulate_vibrato, 0, Dither::FULL},
                {&SYNTH::modulate_tremelo, 98, Dither::LOW},
                {&SYNTH::modulate_vector, 98, Dither::HALF},
                {&ARP::set_range, 98, Dither::OFF}
            };

            // int8_t int8_output (int16_t input) {
            //     int8_t format = static_cast<char>((input + 32767) >> 9) - 128; // casts a signed 16 bit int to a signed 8 bit int
            //     int8_t scaled = (format * _depth) >> 10; // scales the output with a 10 bit depth control
            //     return scaled;
            // }
            // uint16_t uint10_output (int16_t input) {
            //     // uint16_t format = ; // casts a signed 16 bit int to a unsigned 10 bit int
            //     // uint16_t scaled = ; // scales the output with a 10 bit depth control
            //     // return ( * _depth) >> 10;
            //     return ((input + 32767) >> 6);
            // }
            uint16_t uint16_output (int16_t input) {
                return (input + 32767);
            }
            uint16_t prng() {
                // Update the seed with the ADC::noise() value
                prng_seed += ADC::noise();
                // Perform some simple bitwise operations on the seed to generate a pseudo-random number
                prng_seed ^= (prng_seed << 7);
                prng_seed ^= (prng_seed >> 9);
                prng_seed ^= (prng_seed << 8);
                return prng_seed;
            }
            void reset_destination (uint8_t index) {
                volatile int16_t temp = 0;
                if (index == 0) {
                    _destination[index].variable(uint16_output(temp));
                }
                else {
                    _destination[index].variable(0);
                }
            }
            void reset (void) {
                _index = 0;
                _phase_accumulator = 0;

                for (int i = 0; i < 3; i++) {
                    reset_destination(i);
                }
            }
        public:
            Modulation (uint32_t sample_rate) : _sample_rate(sample_rate) { }
            ~Modulation( ) { }

            // control and update functions
            void set_state (bool state) {
                if (_state != state) {
                    _state = state;
                    reset(); // aim to use this to ramp down values when switching states
                }
            }
            bool get_state (void) {
                return _state;
            }
            void set_matrix (uint16_t matrix) {
                volatile uint8_t temp = (matrix>>8);
                if (_matrix != temp) {
                    // Store the current matrix value
                    uint8_t prevMatrix = _matrix;
                    _matrix = temp;

                    _index = 0;
                    _phase_accumulator = 0;
                    
                    // Update the previous destination output to the offset position
                    reset_destination(prevMatrix);
                    reset_destination(_matrix);
                }
            }
            void set_rate (uint16_t rate) {
                double _rate = rate * RATE_SCALE_FACTOR; // Convert 10-bit rate value to a rate between 0.01 Hz and the desired maximum frequency

                _increment = (65535 * _rate) / (_sample_rate * RATE_SCALE_FACTOR); // Calculate the increment based on the scaled rate
                if (_increment < 1) _increment = 1;
            }
            void set_depth (uint16_t depth) {
                _depth = depth; // 0-1023
            }
            void set_wave (uint16_t wave) {
                volatile uint16_t temp = ((wave>>6)*256);
                if (_wave != temp) {
                    _wave = temp;
                }
            }
            void update () {
                if (_state) {

                    _phase_accumulator += _increment; // Adds the increment to the accumulator
                    _index = ((_phase_accumulator >> 16) + _destination[_matrix].offset); // Calculates the 8 bit index value for the wavetable and adds the offset
                    _sample = wavetable[_index + _wave]; // Sets the wavetable value to the sample by using a combination of the index (0-255) and wave (steps of 256) values
                    
                    switch (_destination[_matrix].dither) {
                        case Dither::FULL:
                            _sample += (prng()>>4); // for Vibrato adds an ADC based dither as the waves are only 8 bit so sounds wierd over 0.01Hz
                            break;
                        case Dither::HALF:
                            _sample += (prng()>>6); // for Vibrato adds an ADC based dither as the waves are only 8 bit so sounds wierd over 0.01Hz
                            break;
                        case Dither::LOW:
                            _sample += ADC::noise(); // for Tremelo/ adds a low dither to the output
                            break;
                        case Dither::OFF:
                            break;
                    }
                    
                    uint16_t temp = 0;
                    if (_matrix == 0) temp = uint16_output((_sample * _depth) >> 10);   //the uint16 recast has to happen like this for pitch (centres round 0)
                    else temp = (uint16_output(_sample) * _depth) >> 10;                // and like this for trem/vector
                    _destination[_matrix].output = temp; // comnvert to an unsigned number apply the depth and then bit shift. gives more reliable results than bit shifting signed number
                    
                    if (_destination[_matrix].variable != NULL) 
                        _destination[_matrix].variable(_destination[_matrix].output);
                }
            }
            void clear (void) {
                reset();
            }
    };




    extern Modulation LFO;

    void init (void);

    void update (void);

    void clear (void);

    void set_matrix (uint16_t input);
    void set_depth (uint16_t input);
    void set_rate (uint16_t input);
    void set_shape (uint16_t input);
    void set_state (bool input);

}



