#pragma once

#include "config.h"

#include "drivers/eeprom.h"


namespace PRESET {

    struct SynthPreset {
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
            
            uint8_t padding = 0;
        };
        
        struct OscillatorData {
            uint16_t shape      = 0;
            uint16_t vector     = 0;

            uint16_t padding[2];
        };

        struct VoiceData {
            OscillatorData  Oscillator1; // 16 bytes
            OscillatorData  Oscillator2; // 16 bytes
            EnvelopeData    Envelope;    // 8 bytes

            uint16_t        octave = 0; 
            uint16_t        pitchBend = 511;
            
            uint16_t        detune = 0;
            uint16_t        noiseLevel = 0;

            uint16_t padding[2];
        };

        struct LfoData {
            uint16_t matrix = 0;
            uint16_t rate = 0;
            uint16_t depth = 0;
            uint16_t wave = 0;

            uint16_t speed = 0;
            uint16_t dither = 0;
            uint16_t spare[2];

            EnvelopeData Envelope;

            uint16_t padding[2];
        };

        struct FilterData {
            uint16_t cutoff = 1023;
            uint16_t resonance = 0;
            uint16_t punch = 0;
            uint16_t type = 0;

            uint16_t direction = 0;
            uint16_t keyTracking = 0;
            uint16_t envelopeDepth = 0;
            uint16_t triggerMode = 0;

            EnvelopeData Envelope;

            uint16_t padding[2];
        };

        struct ArpeggiatorData {
            uint16_t gate = 0;
            uint16_t divisions = 511;
            uint16_t range = 0;
            uint16_t direction = 0;

            uint16_t bpm = 287; // 120bpm
            uint16_t synced = 0;
            uint16_t octaveMode = 0;
            uint16_t playedOrder = 0;

            uint16_t padding[2];
        };

        struct FxData {
            uint16_t gain = 0;

            uint16_t padding[3];
        };
        uint8_t             version; // for checking preset version
        uint8_t             length;

        StateData           States;

        VoiceData           Voice;
        LfoData             Modulation;
        FilterData          Filter;
        ArpeggiatorData     Arpeggiator;
        FxData              Effects;

        uint8_t             padding[2];
    };

    void init(void);
    void save (uint8_t slot, SynthPreset &preset);
    void load (uint8_t slot, SynthPreset &preset);

    void factoryRestore (uint8_t slot);
    void factoryWrite (uint8_t slot);

    void exportViaSysEx(const SynthPreset &preset);

    void printData (SynthPreset &preset);



    namespace {
        void transfer (uint16_t from, uint16_t to) {
            printf("Starting Preset transfer...\n\n");

            uint8_t size = sizeof(SynthPreset);
            uint8_t buffer[size];
        
            // Read the data from the EEPROM address
            EEPROM::read(from, buffer, size);
            EEPROM::write(to, buffer, size);

            printf("Preset transfer complete!\n\n");
        }

    }
}


