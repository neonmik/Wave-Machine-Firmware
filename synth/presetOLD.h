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

        uint16_t matrix = 0;
        uint16_t rate = 0;
        uint16_t depth = 0;
        uint16_t wave = 0;
    };
    struct Arp {
        bool state = false;

        uint16_t gate = 0;
        uint16_t divisions = 511;
        uint16_t range = 0;
        uint16_t direction = 0;

        uint16_t rest = 0;
        uint16_t bpm = 289;
        uint16_t fMode = 0;
        uint16_t octMode = 0;
    };
    struct Flt {
        bool state = true;
        
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

inline void printPresetData (PRESET &preset) {
    printf("====================================================\n");
    printf("                    PRESET DATA                     \n");
    printf("====================================================\n\n");

    printf("Waveshape:      %04d    |        ",      preset.Wave.shape);
        printf("Attack:         %04d\n",      preset.Envelope.attack);
    printf("Vector:         %04d    |        ",      preset.Wave.vector);
        printf("Decay:          %04d\n",      preset.Envelope.decay);
    printf("Octave:         %04d    |        ",      preset.Wave.octave);
        printf("Sustain:        %04d\n",      preset.Envelope.sustain);
    printf("Pitch:          %04d    |        ",    preset.Wave.pitch);
        printf("Release:        %04d\n\n",    preset.Envelope.release);
    

    printf("----------------------------------------------------\n");
    printf("                     FILTER\n");
    printf("----------------------------------------------------\n\n");

    printf("State:          ");
    if (preset.Filter.state == true) { printf("Enabled\n"); }
    else { printf("Disabled\n"); }

    printf("Cutoff:         %04d    |        ",      preset.Filter.cutoff);
        printf("Attack:         %04d\n",      preset.Filter.attack);
    printf("Resonance:      %04d    |        ",      preset.Filter.resonance);
        printf("Decay:          %04d\n",      preset.Filter.decay);
    printf("Punch:          %04d    |        ",      preset.Filter.punch);
        printf("Sustain:        %04d\n",      preset.Filter.sustain);
    printf("Type:           %04d    |        ",    preset.Filter.type);
        printf("Release:        %04d\n\n",    preset.Filter.release);


    printf("----------------------------------------------------\n");
    printf("                      LFO\n");
    printf("----------------------------------------------------\n\n");

    printf("State:          ");
    if (preset.Modulation.state == true) { printf("Enabled\n"); }
    else { printf("Disabled\n"); }

    printf("Destination:    %04d\n",      preset.Modulation.matrix);
    printf("Rate:           %04d\n",      preset.Modulation.rate);
    printf("Depth:          %04d\n",      preset.Modulation.depth);
    printf("Shape:          %04d\n\n",    preset.Modulation.wave);
    

    printf("----------------------------------------------------\n");
    printf("                      ARP\n");
    printf("----------------------------------------------------\n\n");

    printf("State:          ");
    if (preset.Arpeggiator.state == true) { printf("Enabled\n"); }
    else { printf("Disabled\n"); }

    printf("Gate:           %04d    |        ",      preset.Arpeggiator.gate);
        printf("Rest:           %04d\n",      preset.Arpeggiator.rest);
    printf("Rate/Division:  %04d    |        ",      preset.Arpeggiator.divisions);
        printf("BPM:            %04d\n",      preset.Arpeggiator.bpm);
    printf("Depth:          %04d    |        ",      preset.Arpeggiator.range);
        printf("Filter Mode:    %04d\n",      preset.Arpeggiator.fMode);
    printf("Shape:          %04d    |        ",    preset.Arpeggiator.direction);
        printf("Octave Mode:    %04d\n\n",    preset.Arpeggiator.octMode);


    printf("----------------------------------------------------\n");
    printf("                      FX\n");
    printf("----------------------------------------------------\n\n");

    printf("Gain:           %04d\n\n",      preset.Effects.gain);
}


struct EnvelopeData {
    uint16_t attack = 2;
    uint16_t decay = 2;
    uint16_t sustain = 1023;
    uint16_t release = 10;
};

struct StateData {
    bool    stateLFO = false;
    bool    stateFLT = false;
    bool    stateARP = false;
    
    bool    padding = false;
};
struct OscillatorData {
    uint16_t waveshape = 0;
    uint16_t vector = 0;
    uint16_t octave = 0;
    uint16_t pitch = 511;

    EnvelopeData Envelope;

    uint16_t padding[4];
};

struct LfoData {
    uint16_t matrix = 0;
    uint16_t rate = 0;
    uint16_t depth = 0;
    uint16_t wave = 0;

    EnvelopeData Envelope;

    uint16_t padding[4];
    
};

struct FilterData {
    uint16_t cutoff = 1023;
    uint16_t resonance = 0;
    uint16_t punch = 0;
    uint16_t type = 0;

    EnvelopeData Envelope;

    uint16_t padding[4];
};

struct ArpeggiatorData {
    uint16_t gate = 0;
    uint16_t divisions = 511;
    uint16_t range = 0;
    uint16_t direction = 0;

    uint16_t rest = 0;
    uint16_t bpm = 287; // 120bpm
    uint16_t fMode = 0;
    uint16_t octMode = 0;

    uint16_t padding[4];
};

struct FxData {
    uint16_t gain = 0;

    uint16_t padding[7];
};

struct NEW_PRESET {
    uint8_t             version; // for checking preset version
    uint8_t             length;

    StateData           States;

    OscillatorData      Oscillator1;
    LfoData             Modulation;
    FilterData          Filter;
    ArpeggiatorData     Arpeggiator;
    FxData              Effects;

    uint8_t             padding[10];
};

inline void printPresetSizing (void) {
    printf("\nSize of ");
    printf("Old Preset: %04d bytes\n\n", sizeof(PRESET));
    
    printf("Size of ");
    printf("New Preset: %04d bytes\n", sizeof(NEW_PRESET));
    printf("    Size of ");
    printf("Oscillator Data: %04d bytes\n", sizeof(OscillatorData));
    printf("    Size of ");
    printf("LFO Data: %04d bytes\n", sizeof(LfoData));
    printf("    Size of ");
    printf("Filter Data: %04d bytes\n", sizeof(FilterData));
    printf("    Size of ");
    printf("Arp Data: %04d bytes\n", sizeof(ArpeggiatorData));
    printf("    Size of ");
    printf("FX Data: %04d bytes\n\n", sizeof(FxData)); 
}