#pragma once

#include "../config.h"

struct PRESET {
    struct Wav {
        uint16_t shape = 0;
        uint16_t vector = 0;
        uint16_t octave = 0;
        uint16_t pitch = 511;
    };
    struct Env {
        uint16_t attack = 2;
        uint16_t decay = 2;
        uint16_t sustain = 1023;
        uint16_t release = 10;
    };
    struct Lfo {
        bool state = false;
        // 1 byte padding

        uint16_t matrix = 0;
        uint16_t rate = 0;
        uint16_t depth = 0;
        uint16_t wave = 0;

        // commented until they're worked into the Preset saving layout
        
        // uint16_t attack = 2;
        // uint16_t decay = 2;
        // uint16_t sustain = 1023;
        // uint16_t release = 10;
    };
    struct Arp {
        bool state = false;
        // 1 byte padding

        uint16_t gate = 0;
        uint16_t divisions = 511;
        uint16_t range = 0;
        uint16_t direction = 0;

        uint16_t rest = 0;
        uint16_t bpm = 289; // 120bpm
        uint16_t fMode = 0;
        uint16_t octMode = 0;
    };
    struct Flt {
        bool state = true;
        // 1 byte padding
        
        uint16_t cutoff = 1023;
        uint16_t resonance = 0;
        uint16_t punch = 0;
        uint16_t type = 0;

        uint16_t attack = 2;
        uint16_t decay = 2;
        uint16_t sustain = 1023;
        uint16_t release = 1023;
    };
    struct Fx {
        uint16_t gain = 0;
    };
    Wav Wave;
    Env Envelope;
    Lfo Modulation;
    Arp Arpeggiator;
    Flt Filter;
    Fx  Effects;
};

struct NEW_PRESET {
    struct Env {
        bool state = false;
        
        uint16_t attack = 2;
        uint16_t decay = 2;
        uint16_t sustain = 1023;
        uint16_t release = 10;
    };
    struct Wav {
        uint16_t waveshape = 0;
        uint16_t vector = 0;
        uint16_t octave = 0;
        uint16_t pitch = 511;

        Env Envelope;
    };
    struct Lfo {
        bool state = false;
        // 1 byte padding

        uint16_t matrix = 0;
        uint16_t rate = 0;
        uint16_t depth = 0;
        uint16_t wave = 0;

        // commented until they're worked into the Preset saving layout

        Env Envelope;
        
        // uint16_t attack = 2;
        // uint16_t decay = 2;
        // uint16_t sustain = 1023;
        // uint16_t release = 10;
    };
    struct Arp {
        bool state = false;
        // 1 byte padding

        uint16_t gate = 0;
        uint16_t divisions = 511;
        uint16_t range = 0;
        uint16_t direction = 0;

        uint16_t rest = 0;
        uint16_t bpm = 289; // 120bpm
        uint16_t fMode = 0;
        uint16_t octMode = 0;
    };
    struct Flt {
        bool state = true;
        // 1 byte padding
        
        uint16_t cutoff = 1023;
        uint16_t resonance = 0;
        uint16_t punch = 0;
        uint16_t type = 0;

        Env Envelope;
    };
    struct Fx {
        uint16_t gain = 0;
    };
    Wav Wave;
    Env Envelope;
    Lfo Modulation;
    Arp Arpeggiator;
    Flt Filter;
    Fx  Effects;
};