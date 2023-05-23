#pragma once

#include "pico/stdlib.h"
#include <math.h>

#include "../random.h"

#include "../functions.h"

#include "synth.h"
#include "arp.h"

#include "wavetable.h"


// Wave table size
#define WAVE_TABLE_SIZE 256

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
            // main info
            uint32_t _sample_rate;

            // oscillator variables
        
            uint32_t _increment;
            uint32_t _phase_accumulator;
            uint16_t _phase_fractional;
            uint8_t _index;
            int16_t  _sample;

            uint32_t _frequency;
            const double RATE_SCALE_FACTOR = 0.1; // Define the scaling factor to convert 10-bit value to rate in Hz
            
            // control variables
            bool     _state = false;
            uint8_t  _matrix;
            uint16_t _rate;
            uint16_t _depth;
            uint16_t  _wave;
            
            OutputDestinations _destination[4]{
                // pointer of what to update, type of output, offset for output table
                {&SYNTH::modulate_vibrato,  OutputType::SIGNED,     Dither::FULL},
                {&SYNTH::modulate_tremelo,  OutputType::UNSIGNED,   Dither::LOW},
                {&SYNTH::modulate_vector,   OutputType::UNSIGNED,   Dither::LOW},
                {&ARP::set_range,           OutputType::SIGNED,   Dither::OFF} // probably want something better here, but we'll see
            };
            uint16_t uint16_output (int16_t input) {
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
                // 0.1Hz - 100Hz
                // 1 = 0.1Hz / 1024 = 102.4Hz
                
                _rate = (map_exp(rate, 0, 1023, 1, 5000));
                
                

                _increment = (65535 * _rate) / (_sample_rate); // Calculate the increment based on the scaled rate

                if (_increment < 1) _increment = 1;
                
                // printf("Rate: %fHz\n", _rate);
            }
            void set_depth (uint16_t depth) {
                _depth = depth; // 0-1023
            }
            void set_shape (uint16_t wave) {
                volatile uint16_t temp = ((map(wave, 0, 1023, 0, 5))*256);
                if (_wave != temp) {
                    _wave = temp;
                }
            }
            
            void update () {
                if (_state) {

                    _phase_accumulator += _increment; // Adds the increment to the accumulator
                    _index = (_phase_accumulator >> 16); // Calculates the 8 bit index value for the wavetable and adds the offset
                    // printf("index: %d\n", _index);
                    _sample = get_mod_wavetable(_index + _wave); // Sets the wavetable value to the sample by using a combination of the index (0-255) and wave (steps of 256) values
                    // really just for smoothing out 8 bit numbers over 0.01Hz

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
                    

                    // two different algoruthms for applying depth to the outputs, ensures always the number is centred round teh respective 0 mark for the destination. 
                    switch (_destination[_matrix].type) {
                        case OutputType::UNSIGNED:
                        {
                            _destination[_matrix].output = (uint16_output(_sample) * _depth) >> 10;
                            break;
                        }
                        case OutputType::SIGNED:
                        {
                            _destination[_matrix].output = uint16_output((_sample * _depth) >> 10);
                            break;
                        }
                    }

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

    void set_state (bool input);

    void set_matrix (uint16_t input);
    void set_depth (uint16_t input);
    void set_rate (uint16_t input);
    void set_shape (uint16_t input);

    void update (void);
    void clear (void);
}



