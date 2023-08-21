#pragma once

#include "pico/stdlib.h"

#include <math.h>

#include "../config.h"
#include "../functions.h"
#include "../queue.h"

#include "resources.h"

#include "adsr.h"
#include "fx.h"

namespace FILTER {

    enum Type {
        Off,
        LowPass,
        BandPass,
        HighPass
    };

    enum Direction {
        Regular,
        Inverted
    };

    namespace {
        uint8_t     _index;

        bool        _dirty;

        int16_t     _cutoff;
        int16_t     _resonance;

        uint16_t    _mod;

        int32_t     _punch;
        int32_t     _frequency;
        int32_t     _damp;

        int32_t     _lowpass;
        int32_t     _bandpass;

        uint32_t    _attack;
        uint32_t    _decay;
        uint32_t    _sustain;
        uint32_t    _release;
        uint16_t    _last_attack = 1024;
        uint16_t    _last_decay = 1024;
        uint16_t    _last_sustain = 1024;
        uint16_t    _last_release = 1024;

        Type        _mode;
        Direction   _direction;

        inline uint16_t Interpolate824(const uint16_t* table, uint32_t phase) {
            uint32_t a = table[phase >> 24];
            uint32_t b = table[(phase >> 24) + 1];
            return a + ((b - a) * static_cast<uint32_t>((phase >> 8) & 0xffff) >> 16);
        }

        inline int16_t Interpolate824(const uint8_t* table, uint32_t phase) {
            int32_t a = table[phase >> 24];
            int32_t b = table[(phase >> 24) + 1];
            return (a << 8) + \
                ((b - a) * static_cast<int32_t>(phase & 0xffffff) >> 16) - 32768;
        }

        uint32_t calc_end_frame (uint32_t milliseconds) {
            // return (milliseconds * (SAMPLE_RATE/8)) / 1000;
            return (milliseconds * SAMPLE_RATE) / 1000;
        }


    }
    

    extern ADSREnvelope ADSR;

    void Init();
    void set_cutoff(uint16_t frequency);
    void set_resonance(uint16_t resonance);
    void set_punch(uint16_t punch);
    void set_mode(uint16_t mode);
    void process(int32_t &sample);

    void set_attack (uint16_t attack);
    void set_decay (uint16_t decay);
    void set_sustain (uint16_t sustain);
    void set_release (uint16_t release);

    void trigger_attack (void);
    void trigger_release (void);

    void modulate_cutoff(uint16_t cutoff);
    void modulate_resonance(uint16_t resonance);

}
