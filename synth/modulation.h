#pragma once

#include "pico/stdlib.h"

#include "synth.h"
#include "../mailbox.h"

#include "wavetable.h"

// Wave table size
#define WAVE_TABLE_SIZE 256

namespace MOD {


    enum Matrix : uint8_t{
        OFF = 0,
        VIBRATO = 1,
        TREM = 2,
        VECTOR = 3
    };

    
    class Modulation {
        private:

            bool        _active; 

            uint8_t              _index       = 0;
            uint32_t             _increment   = 0;
            uint16_t             _phase_acc   = 0;

            int16_t              _sample      = 0;

            uint16_t             _vibrato     = 0;
            uint16_t             _trem        = 0;
            uint16_t             _vector      = 0;


            uint8_t              _matrix      = Matrix::OFF;
            uint8_t              _last_matrix = 0;
            uint16_t             _depth       = 0;
            volatile uint16_t    _rate        = 0;
            uint16_t             _wave        = 0;
            uint16_t             _last_wave   = 0;


            uint16_t              _sample_rate = 750;   // Sample Rate (44100Hz) / 64 = 689.0625... this only gets called every 64 samples at the main loop
                                                        // sample Rate (48000Hz) / 64 = 750
            
        public:
            Modulation() { //uint16_t sample_rate) {
                // _sample_rate = (sample_rate/64);
            }
            ~Modulation() { }
            
            void init (void) {
                set_depth(0);
                set_rate(0);
                set_wave(0);
            }
            
            void set_matrix (uint16_t input) {
                uint8_t temp = (input >> 8);
                if (temp != _last_matrix) {
                    _matrix = temp;
                    _last_matrix = temp;
                    init(); // clear previous results
                }
            }

            void set_depth (uint16_t input) {
                if (_matrix == Matrix::VIBRATO) _depth = (input>>4); //not right
                if (_matrix == Matrix::TREM) _depth = (input); // PERFECT
                if (_matrix == Matrix::VECTOR) _depth = (input>>4); // Not right
                else {
                    _depth = input;
                }
            }

            void set_rate (uint16_t input) {
                // ((uint16_t wrap around * Hz+1(must always be above 0hz)) / 187.5 (sample rate for lfo (48k / 256)) >> 6 (divide by 64 to get sub Hz freq without the floating point hit)
                _increment = ((65535 * (input+1)) / _sample_rate) >> 6; 
            }

            void set_wave (uint16_t input) {
                _wave = ((input>>6)*256);
            }

            void set_state (bool input) {
                _active = input;
                if (!_active) {
                    _vibrato = 0;
                    _trem = 0;
                    _vector = 0;
                    SYNTH::_vibrato = _vibrato;
                    SYNTH::_tremelo = _trem;
                    SYNTH::_vector_mod = _vector;
                }
            }

            bool get_state (void) {
                return _active;
            }

            void clear (void) {
                set_state(false);
                init();
                _vibrato = 0;
                _trem = 0;
                _vector = 0;
            }

            void update (void) {
                if (_active) {
                    _phase_acc += _increment;
                    _index = (_phase_acc>>8);
                    _sample = (((wavetable[_wave + _index]) * _depth) >> 10);
                    switch (_matrix) {
                        case Matrix::OFF:
                            _vibrato = 0;
                            _trem = 0;
                            _vector = 0;
                            break;
                        case Matrix::VIBRATO:
                            _vibrato = (_sample>>8); // set to +/-16 as useful range at fast and slow, but +/-128 and upwards (+/-256 or +/-512) is useful for *weird*. needs Log function.
                            _trem = 0;
                            _vector = 0;
                            break;
                        case Matrix::TREM:  
                            _vibrato = 0;
                            _trem = uint16_t(uint32_t(_sample)+32767);
                            _vector = 0;
                            break;
                        case Matrix::VECTOR:
                            _vibrato = 0;
                            _trem = 0;
                            _vector = (uint16_t(uint32_t(_sample)+32767))>>4; // (>> 8 is 0-255, >> 6 is 0-1023, >>4 is 0-4095)
                            break;
                        default:
                            _vibrato = 0;
                            _trem = 0;
                            _vector = 0;
                            break;
                    }
                    SYNTH::_vibrato = _vibrato;
                    SYNTH::_tremelo = _trem;
                    SYNTH::_vector_mod = _vector;
                }
                else {
                    _vibrato = 0;
                    _trem = 0;
                    _vector = 0;
                    SYNTH::_vibrato = _vibrato;
                    SYNTH::_tremelo = _trem;
                    SYNTH::_vector_mod = _vector;
                }
            }
    };

// LFO/Modulation class
// class Lfo {
// public:
//     // Constructor
//     Lfo (float sampleRate) {
//         // Calculate frequency increment based on sample rate
//         frequencyIncrement = (WAVE_TABLE_SIZE / sampleRate) * (1 << 10);
//         frequency = 0.0; // Initialize frequency to 0 Hz
//     }
//     // Function to calculate the modulation value
//     int16_t process(bool isSigned) {
//         static float phase = 0.0;
//         int16_t output = 0;
//         // Update phase
//         phase += frequency * frequencyIncrement;
//         if (phase >= WAVE_TABLE_SIZE) {
//             phase -= WAVE_TABLE_SIZE;
//         }
//         // Read waveform value from wave table
//         int8_t waveValue = wavetable[(int)phase];
//         // Scale output to 10-bit range
//         if (isSigned) {
//             // If signed output, scale to -512 to +511
//             output = (waveValue * 511) / (1 << 7);
//         } else {
//             // If unsigned output, scale to 0 to 1023
//             output = ((waveValue + (1 << 7)) / 2) * 2; // Round to nearest even number
//         }
//         return output;
//     }
//     // Function to set the frequency of the LFO
//     void set_frequency(float freq) {
//         frequency = freq;
//     }
// private:
//     float frequency;          // Frequency of the LFO
//     float frequencyIncrement; // Frequency increment per sample
// };


    extern Modulation LFO;


    void init (uint16_t sample_rate);

    void update (void);

    void clear (void);

    void set_matrix (uint16_t input);
    void set_depth (uint16_t input);
    void set_rate (uint16_t input);
    void set_shape (uint16_t input);
    void set_state (bool input);
}
