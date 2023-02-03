#pragma once

#include "pico/stdlib.h"

#include "synth.h"

#include "wavetable.h"

// This wants to be a LFO for modulating different sources...
// I think it should be between 1Hz-20Hz (Or ???100Hz???)
// Could be used for:
//                      - Vibrato (Note frequency modulations)
//                      - Tremelo (Note Volume Modulations)
//                      - Wave Vector Modulations
//                      - A Filter?

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

            uint16_t    _values[4];

            uint8_t              _index       = 0;
            uint32_t             _increment   = 0;
            uint16_t             _phase_acc   = 0;

            int16_t              _sample      = 0;
            int16_t              _vibrato     = 0;
            uint16_t             _trem        = 0;
            uint8_t              _vector      = 0;


            uint16_t             _depth       = 0;
            volatile uint16_t    _rate        = 0;

            uint16_t             _wave        = 0;
            uint16_t             _last_wave   = 0;

            uint8_t              _matrix      = Matrix::OFF;
            uint8_t              _last_matrix = 0;

            uint8_t              _sample_rate = 172; // Sample Rate (44100Hz) / 256 = 172.26... this only gets called every 256 samples at the main loop
            
        public:
            Modulation() { }
            ~Modulation() { }
            
            void init (void) {
                set_depth(0);
                set_rate(0);
                set_wave(0);
            }
            void set_matrix (uint16_t input) {
                _values[0] = input;
                uint8_t temp = (input >> 8);
                if (temp != _last_matrix) {
                    _matrix = temp;
                    _last_matrix = temp;
                    init(); // clear previous results
                }

            }
            uint16_t get_matrix (void) {
                return _values[0];
            }

            void set_depth (uint16_t input) {
                _values[1] = input;
                if (_matrix == Matrix::VIBRATO) _depth = (input>>4); //not right
                if (_matrix == Matrix::TREM) _depth = (input); // PERFECT
                if (_matrix == Matrix::VECTOR) _depth = (input>>4); // Not right
                else {
                    _depth = input;
                }
            }
            uint16_t get_depth (void) {
                return _values[1];
            }

            void set_rate (uint16_t input) {
                _values[2] = input;
                // ((uint16_t wrap around * Hz) / 187.5 (sample rate for lfo (48k / 256)) >> 6 (divide by 64 to get sub Hz freq without the floating point hit)
                _increment = ((65535 * (input+1)) / _sample_rate) >> 6; 
            }
            uint16_t get_rate (void) {
                return _values[2];
            }

            void set_wave (uint16_t input) {
                _values[3] = input;
                _wave = ((input>>6)*256);
            }
            uint16_t get_wave (void) {
                return _values[3];
            }

            
            void start (void) {
                _active = true;
            }
            void toggle (void) {
                _active = !_active;
            }
            void stop (void) {
                _active = false;
            }
            void clear (void) {
                stop();
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
                    if (_matrix == Matrix::OFF) {
                        _vibrato = 0;
                        _trem = 0;
                        _vector = 0;
                    }
                    if (_matrix == Matrix::VIBRATO) {
                        _vibrato = (_sample>>8); // set to +/-16 as useful range at fast and slow, but +/-128 and upwards (+/-256 or +/-512) is useful for *weird*. needs Log function.
                        _trem = 0;
                        _vector = 0;
                    }
                    if (_matrix == Matrix::TREM) {
                        _vibrato = 0;
                        _trem = uint16_t(uint32_t(_sample)+32767);
                        _vector = 0;
                    }
                    if (_matrix == Matrix::VECTOR) {
                        _vibrato = 0;
                        _trem = 0;
                        _vector = (uint16_t(uint32_t(_sample)+32767))>>6; // (>> 8 is 0-255, >> 6 is 0-1023, >>4 is 0-4095)

                    }
                    synth::vibrato = _vibrato;
                    synth::tremelo = _trem;
                    synth::vector_mod = _vector;
                }
                else {
                    synth::vibrato = 0;
                    synth::tremelo = 0;
                    synth::vector_mod = 0;
                }
                
            }
    };

    extern Modulation MOD1;
    // extern Modulation MOD2;

    void init (void);

    void update (void);

    void clear (void);
}

