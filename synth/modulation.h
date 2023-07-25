#pragma once

#include "pico/stdlib.h"
#include <math.h>

#include "../random.h"

#include "../functions.h"

#include "synth.h"
#include "arp.h"
#include "filter.h"

#include "wavetable.h"

namespace MOD {

    enum Dither : uint8_t {
        FULL,
        HALF,
        LOW,
        OFF
    };

    enum OutputType : uint8_t {
        SIGNED,
        UNSIGNED
    };

    struct OutputDestinations {
        void            (*variable)(uint16_t);
        OutputType      type;
        Dither          dither;
        uint16_t        output = 0;
    };

    class Modulation {
        private:
            // oscillator variables

            uint32_t    _sample_rate;

            uint32_t    _increment;
            uint32_t    _phase_accumulator;
            uint8_t     _index;

            int16_t     _sample;

            
            // control variables

            bool        _state = false;
            uint8_t     _matrix;
            uint16_t    _rate;
            uint16_t    _depth;
            uint16_t    _wave;
            
            OutputDestinations _destination[4]{
                // pointer of what to update, type of output, offset for output table
                {&SYNTH::modulate_vibrato,  OutputType::SIGNED,     Dither::FULL},
                {&SYNTH::modulate_tremelo,  OutputType::UNSIGNED,   Dither::LOW},
                {&SYNTH::modulate_vector,   OutputType::UNSIGNED,   Dither::LOW},
                {nullptr,                   OutputType::UNSIGNED,   Dither::OFF} // probably want something ? here, but we'll see...
            };

            inline uint16_t uint16_output (int16_t input) {
                return input - INT16_MIN; // Using modular arithmatic!
            }

            void reset_destination (uint8_t index) {
                if (_destination[index].variable != NULL) {
                    switch (_destination[index].type) {
                        case OutputType::UNSIGNED:
                            _destination[index].variable(0);
                            break;
                        case OutputType::SIGNED:
                            _destination[index].variable(uint16_output(0));
                    }
                }
            }

            void reset (void) {
                _index = 0;
                _phase_accumulator = 0;
                _sample = 0;

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
                // uses a LUT for expoentially mapping 0-1023 to 1-10000 - giving us 0.1Hz to 1000Hz freq control. this saves a chunk of processing from calculating every time.
                // so far, the measured response is 0.0028Hz - 48Hz I think! maybe need to adjust this. 
                _rate = exp_freq(rate);
                
                // Calculate the increment based on the scaled rate
                _increment = (65535 * (uint32_t)_rate) / _sample_rate;

                // don't want the increment dropping below 1, as it will stop the oscillation/cause weird behaviour
                if (_increment < 1) _increment = 1;
            }
            void set_depth (uint16_t depth) {
                if (_depth != depth) {
                    // 10 bit depth setting
                    _depth = depth;
                }
            }
            void set_shape (uint16_t wave) {
                // map the 10 bit knob value to 0-5 (the amount of waveforms) and then map it to the wavetable size.
                volatile uint16_t mapped_wave = (map(wave, KNOB_MIN, KNOB_MAX, 0, (MAX_MOD_WAVES - 1)) * WAVETABLE_SIZE);
                if (_wave != mapped_wave) {
                    _wave = mapped_wave;
                }
            }
            
            void update () {
                if (_state) {
                    _phase_accumulator += _increment; // Adds the increment to the accumulator
                    _index = (_phase_accumulator >> LFO_SPEED::SLOW);    // Calculates the 8 bit index value for the wavetable. the bit shift creates diffeing results... see LFO_SPEED table
                    _sample = get_mod_wavetable(_index + _wave); // Sets the wavetable value to the sample by using a combination of the index (0-255) and wave (chunks of 256) values
                    
                    // Applies a certain dither to the output - really just for smoothing out 8 bit numbers over 0.01Hz, but interesting for effects.
                    switch (_destination[_matrix].dither) {
                        case Dither::FULL:
                            if (_sample > 0) _sample -= (RANDOM::get()>>4);
                            else _sample += (RANDOM::get()>>4);
                            break;
                        case Dither::HALF:
                            if (_sample > 0) _sample -= (RANDOM::get()>>6);
                            else _sample += (RANDOM::get()>>6);
                            break;
                        case Dither::LOW:
                            if (_sample > 0) _sample -= (RANDOM::get()>>7);
                            else _sample += (RANDOM::get()>>7);
                            break;
                        case Dither::OFF:
                            break;
                    }
                    

                    // two different algorithms for applying depth to the outputs, ensures always the number is centred round the appropriate 0 mark for the destination. 
                    switch (_destination[_matrix].type) {
                        case OutputType::UNSIGNED: {
                            _destination[_matrix].output = (uint16_output(_sample) * _depth) >> 10;
                            break;
                        }
                        case OutputType::SIGNED: {
                            _destination[_matrix].output = uint16_output((_sample * _depth) >> 10);
                            break;
                        }
                    }

                    if (_destination[_matrix].variable != NULL)
                            if (_depth) _destination[_matrix].variable(_destination[_matrix].output);
                }
            }
            void clear (void) {
                reset();
            }
    };

    extern Modulation LFO;

    void init (void);

    void set_state (bool input);

    void set_matrix (uint16_t input);
    void set_depth (uint16_t input);
    void set_rate (uint16_t input);
    void set_shape (uint16_t input);

    void update (void);
    void clear (void);
}



