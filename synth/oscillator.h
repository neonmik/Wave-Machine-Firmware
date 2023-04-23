#pragma once

#include "pico/stdlib.h"

#include "wavetable.h"
#include "log_table.h"

const int16_t sine_waveform[256] = {-32768,-32758,-32729,-32679,-32610,-32522,-32413,-32286,-32138,-31972,-31786,-31581,-31357,-31114,-30853,-30572,-30274,-29957,-29622,-29269,-28899,-28511,-28106,-27684,-27246,-26791,-26320,-25833,-25330,-24812,-24279,-23732,-23170,-22595,-22006,-21403,-20788,-20160,-19520,-18868,-18205,-17531,-16846,-16151,-15447,-14733,-14010,-13279,-12540,-11793,-11039,-10279,-9512,-8740,-7962,-7180,-6393,-5602,-4808,-4011,-3212,-2411,-1608,-804,0,804,1608,2411,3212,4011,4808,5602,6393,7180,7962,8740,9512,10279,11039,11793,12540,13279,14010,14733,15447,16151,16846,17531,18205,18868,19520,20160,20788,21403,22006,22595,23170,23732,24279,24812,25330,25833,26320,26791,27246,27684,28106,28511,28899,29269,29622,29957,30274,30572,30853,31114,31357,31581,31786,31972,32138,32286,32413,32522,32610,32679,32729,32758,32767,32758,32729,32679,32610,32522,32413,32286,32138,31972,31786,31581,31357,31114,30853,30572,30274,29957,29622,29269,28899,28511,28106,27684,27246,26791,26320,25833,25330,24812,24279,23732,23170,22595,22006,21403,20788,20160,19520,18868,18205,17531,16846,16151,15447,14733,14010,13279,12540,11793,11039,10279,9512,8740,7962,7180,6393,5602,4808,4011,3212,2411,1608,804,0,-804,-1608,-2411,-3212,-4011,-4808,-5602,-6393,-7180,-7962,-8740,-9512,-10279,-11039,-11793,-12540,-13279,-14010,-14733,-15447,-16151,-16846,-17531,-18205,-18868,-19520,-20160,-20788,-21403,-22006,-22595,-23170,-23732,-24279,-24812,-25330,-25833,-26320,-26791,-27246,-27684,-28106,-28511,-28899,-29269,-29622,-29957,-30274,-30572,-30853,-31114,-31357,-31581,-31786,-31972,-32138,-32286,-32413,-32522,-32610,-32679,-32729,-32758};


enum OscillatorMode {
    WAVETABLE = 256,
    NOISE     = 128,
    SQUARE    = 64,
    SAW       = 32,
    TRIANGLE  = 16,
    SINE      = 8,
    WAVE      = 1
  };


class Oscillator {
    private:
        uint16_t&   _sample_rate;                   // sample rate of the synth

        uint16_t&   _oscillator;                    // bitmask for enabled oscillator types (see OscillatorMode enum for values)
        
        uint16_t&   _frequency;                     // frequency of oscillator
        uint8_t&    _octave;                        // octave of oscillator

        uint16_t    _pulse_width        = 0x7fff;   // duty cycle of square wave (default 50%)
        int16_t     _noise              = 0;        // current noise value
        uint32_t    _waveform_offset    = 0;        // voice offset (Q8)

        uint16_t&    _pitch_bend;                   // pitch bend amount
        uint16_t&    _vibrato;                      // vibrato amount

        uint16_t&    _wavetable_index;              // wavetable offset (Q8)

    public:
        Oscillator(uint16_t& sample_rate, uint16_t& oscillator, uint16_t& pitch_bend, uint16_t& wavetable_index, uint16_t& vibrato) :
            _sample_rate(sample_rate),
            _oscillator(oscillator),
            _pitch_bend(pitch_bend),
            _wavetable_index(wavetable_index),
            _vibrato(vibrato)
        { };
        ~Oscillator() { };

        void init (void);
        
        void set (uint16_t frequency, uint8_t octave);
        void clear (void);
        
        int32_t process (void);
};

