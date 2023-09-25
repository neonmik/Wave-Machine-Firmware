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

    enum Mode {
        MONO,
        PARA,
    };

    namespace {
        Mode        _mode  = Mode::PARA;

        uint8_t     _index;

        bool        needsUpdating;

        int16_t     _cutoff;
        int16_t     _resonance;

        uint16_t    _mod;

        int32_t     _punch;
        int32_t     _frequency;
        int32_t     _damp;

        int32_t     lowPass;
        int32_t     bandPass;

        uint32_t    _attack;
        uint32_t    _decay;
        uint32_t    _sustain;
        uint32_t    _release;
        uint16_t    lastAttack = 1024;
        uint16_t    lastDecay = 1024;
        uint16_t    lastSustain = 1024;
        uint16_t    lastRelease = 1024;

        Type        _type;
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

        uint32_t calculateEndFrame (uint32_t milliseconds) {
            // return (milliseconds * (SAMPLE_RATE/8)) / 1000;
            return ((milliseconds + 1) * SAMPLE_RATE) / 1000;
        }

        volatile int8_t     _voices_active;
        bool       filterActive = false;

        void        voices_inc (void) {
            ++_voices_active;
            if (_voices_active > POLYPHONY) {
                _voices_active = POLYPHONY;
            }
        }
        void        voices_dec (void) {
            --_voices_active;
            if (_voices_active < 0) {
                _voices_active = 0;
            }
        }
        void        voices_clr (void) {
            _voices_active = 0;
        }
        bool        voicesActive (void) {
            return _voices_active > 0;
        }


    }
    

    extern ADSREnvelope ADSR;

    void init();
    void setCutoff(uint16_t frequency);
    void setResonance(uint16_t resonance);
    void setPunch(uint16_t punch);
    void setType(uint16_t type);
    void process(int32_t &sample);

    void setAttack (uint16_t attack);
    void setDecay (uint16_t decay);
    void setSustain (uint16_t sustain);
    void setRelease (uint16_t release);

    void voicesIncrease (void);
    void voicesDecrease (void);

    void triggerAttack (void);
    void triggerRelease (void);

    void modulateCutoff(uint16_t cutoff);
    void modulate_resonance(uint16_t resonance);

}
