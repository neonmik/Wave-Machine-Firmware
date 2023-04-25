#pragma once

#include "pico/stdlib.h"

#include "../drivers/adc.h"

#include "synth.h"

#include "wavetable.h"
// Wave table size
#define WAVE_TABLE_SIZE 256

uint8_t ADC::noise();
    /*
    // enum Matrix : uint8_t{
    //     OFF = 0,
    //     VIBRATO = 1,
    //     TREM = 2,
    //     VECTOR = 3
    // };

    
    // class Modulation {
    //     private:

    //         bool        _active; 

    //         uint8_t              _index       = 0;
    //         uint32_t             _increment   = 0;
    //         uint16_t             _phase_acc   = 0;

    //         int16_t              _sample      = 0;

    //         uint16_t             _vibrato     = 0;
    //         uint16_t             _trem        = 0;
    //         uint16_t             _vector      = 0;


    //         uint8_t              _matrix      = Matrix::OFF;
    //         uint8_t              _last_matrix = 0;
    //         uint16_t             _depth       = 0;
    //         volatile uint16_t    _rate        = 0;
    //         uint16_t             _wave        = 0;
    //         uint16_t             _last_wave   = 0;


    //         uint16_t              _sample_rate = 750;   // Sample Rate (44100Hz) / 64 = 689.0625... this only gets called every 64 samples at the main loop
    //                                                     // sample Rate (48000Hz) / 64 = 750
            
    //     public:
    //         Modulation() { //uint16_t sample_rate) {
    //             // _sample_rate = (sample_rate/64);
    //         }
    //         ~Modulation() { }
            
    //         void init (void) {
    //             set_depth(0);
    //             set_rate(0);
    //             set_wave(0);
    //         }
            
    //         void set_matrix (uint16_t input) {
    //             uint8_t temp = (input >> 8);
    //             if (temp != _last_matrix) {
    //                 _matrix = temp;
    //                 _last_matrix = temp;
    //                 init(); // clear previous results
    //             }
    //         }

    //         void set_depth (uint16_t input) {
    //             if (_matrix == Matrix::VIBRATO) _depth = (input>>4); //not right
    //             if (_matrix == Matrix::TREM) _depth = (input); // PERFECT
    //             if (_matrix == Matrix::VECTOR) _depth = (input>>4); // Not right
    //             else {
    //                 _depth = input;
    //             }
    //         }

    //         void set_rate (uint16_t input) {
    //             // ((uint16_t wrap around * Hz+1(must always be above 0hz)) / 187.5 (sample rate for lfo (48k / 256)) >> 6 (divide by 64 to get sub Hz freq without the floating point hit)
    //             _increment = ((65535 * (input+1)) / _sample_rate) >> 6; 
    //         }

    //         void set_wave (uint16_t input) {
    //             _wave = ((input>>6)*256);
    //         }

    //         void set_state (bool input) {
    //             _active = input;
    //             if (!_active) {
    //                 _vibrato = 0;
    //                 _trem = 0;
    //                 _vector = 0;
    //                 SYNTH::_vibrato = _vibrato;
    //                 SYNTH::_tremelo = _trem;
    //                 SYNTH::_vector_mod = _vector;
    //             }
    //         }

    //         bool get_state (void) {
    //             return _active;
    //         }

    //         void clear (void) {
    //             set_state(false);
    //             init();
    //             _vibrato = 0;
    //             _trem = 0;
    //             _vector = 0;
    //         }

    //         void update (void) {
    //             if (_active) {
    //                 _phase_acc += _increment;
    //                 _index = (_phase_acc>>8);
    //                 _sample = (((wavetable[_wave + _index]) * _depth) >> 10);
    //                 switch (_matrix) {
    //                     case Matrix::OFF:
    //                         _vibrato = 0;
    //                         _trem = 0;
    //                         _vector = 0;
    //                         break;
    //                     case Matrix::VIBRATO:
    //                         _vibrato = (_sample>>8); // set to +/-16 as useful range at fast and slow, but +/-128 and upwards (+/-256 or +/-512) is useful for *weird*. needs Log function.
    //                         _trem = 0;
    //                         _vector = 0;
    //                         break;
    //                     case Matrix::TREM:  
    //                         _vibrato = 0;
    //                         _trem = uint16_t(uint32_t(_sample)+32767);
    //                         _vector = 0;
    //                         break;
    //                     case Matrix::VECTOR:
    //                         _vibrato = 0;
    //                         _trem = 0;
    //                         _vector = (uint16_t(uint32_t(_sample)+32767))>>4; // (>> 8 is 0-255, >> 6 is 0-1023, >>4 is 0-4095)
    //                         break;
    //                     default:
    //                         _vibrato = 0;
    //                         _trem = 0;
    //                         _vector = 0;
    //                         break;
    //                 }
    //                 SYNTH::_vibrato = _vibrato;
    //                 SYNTH::_tremelo = _trem;
    //                 SYNTH::_vector_mod = _vector;
    //             }
    //             else {
    //                 _vibrato = 0;
    //                 _trem = 0;
    //                 _vector = 0;
    //                 SYNTH::_vibrato = _vibrato;
    //                 SYNTH::_tremelo = _trem;
    //                 SYNTH::_vector_mod = _vector;
    //             }
    //         }
    // };
    */
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
        OutputTypes     type;
        uint8_t         offset;
        Dither          dither;
        uint16_t        output = 0;

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
            uint32_t _scaled_frequency;
            uint8_t  _freq_fxpt = 100;
            // control variables
            bool     _state = false;
            uint8_t  _matrix;
            uint16_t _rate;
            uint16_t _depth;
            uint8_t  _wave;

            uint16_t prng_seed = 0;

            OutputParams _destination[4]{
                // pointer of what to update, type of output, offset for output table
                {&SYNTH::modulate_vibrato, OutputTypes::UNSIGNED_INT_10, 0, Dither::FULL},
                {&SYNTH::modulate_tremelo, OutputTypes::UNSIGNED_INT_10, 98, Dither::LOW},
                {&SYNTH::modulate_vector, OutputTypes::UNSIGNED_INT_10, 98, Dither::HALF}
            };

            int8_t int8_output (int16_t input) {
                int8_t format = static_cast<char>((input + 32768) >> 9) - 128; // casts a signed 16 bit int to a signed 8 bit int
                int8_t scaled = (format * _depth) >> 10; // scales the output with a 10 bit depth control
                return scaled;
            }
            uint16_t uint10_output (int16_t input) {
                // uint16_t format = ; // casts a signed 16 bit int to a unsigned 10 bit int
                // uint16_t scaled = ; // scales the output with a 10 bit depth control
                // return ( * _depth) >> 10;
                return ((input + 32768) >> 6);
            }
            uint16_t uint16_output (int16_t input) {
                return (input + 32768);
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
            void reset (void) {
                _destination[0].variable(uint16_output(0));
                _destination[1].variable(0);
                _destination[2].variable(0);
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
                _matrix = (matrix>>8) % 3;
            }
            // void set_rate (uint16_t rate) {
            //     _increment = ((65535 * (rate+1)) / _sample_rate) >> 4; 
            // }
            void set_rate (uint16_t rate_10bit) {
                // Define the scaling factor to convert 10-bit value to rate in Hz
                const double RATE_SCALE_FACTOR = 0.01;

                // Convert 10-bit rate value to a rate between 0.01 Hz and the desired maximum frequency
                double rate = rate_10bit * RATE_SCALE_FACTOR;

                // Calculate the increment based on the rate
                _increment = (65535 * rate) / (_sample_rate * RATE_SCALE_FACTOR);

                // Ensure the increment is at least 1
                if (_increment < 1) {
                    _increment = 1;
                }
            }
            void set_depth (uint16_t depth) {
                _depth = depth; // 0-1023
            }
            void set_wave (uint16_t wave) {
                _wave = ((wave>>6)*256); // 
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


                    _destination[_matrix].output = (uint16_output(_sample) * _depth) >> 10;
                    if (_destination[_matrix].variable != NULL) _destination[_matrix].variable(_destination[_matrix].output);
                }
            }
            // void update (void) {
            //     if (_state) {
            //         _phase_accumulator += _increment; // adds the increment to the accumulator

            //         _index = ((_phase_accumulator>>8) + _destination[_matrix].offset); // Calculates the 8 bit index value for the wavetable and adds the offset, done here to keep within the 0-255 range
                    
            //         _sample = wavetable[_index + _wave]; // sets the wavetable value to the sample by using a combination of the index (0-255) and wave (steps of 256) values
                    
            //         _destination[_matrix].output = (uint16_output(_sample) * _depth) >> 10;
            //         if (_destination[_matrix].variable != NULL) _destination[_matrix].variable(_destination[_matrix].output);
            //     }
            // }
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



