#pragma once    

#include "../config.h"

// (((Sample Rate * 60s) / Lowest BPM) / 4) / 24ppqn) = Longest possible dropout 
// #define MIDI_CLOCK_TIMEOUT 670000 // max time in us for 1 pulse at 24ppqn
#define MIDI_CLOCK_TIMEOUT (SAMPLE_RATE * 0.67) // converted value from abov (us to samples)

extern uint32_t sampleClock;

namespace CLOCK {

    namespace {
        uint32_t    clockTick               = 0;
        
        uint8_t     _beat                   = 0;

        bool        clockChanged;
        uint32_t    samplesPerDivision;
        uint32_t    samplesPerPulse;

        uint32_t    sampleRate;
        uint16_t    _bpm                    = 120;
        uint8_t     clockDivision           = 8;

        bool        midiClockPreset         = false;
        uint8_t     midiDivision            = 24;

        uint8_t     currentDivision         = 8;

        uint8_t     midiStartFlag;
        uint8_t     midiStopFlag;
        uint32_t    midiClockLast;
        uint32_t    midiClockPeriod;
        uint8_t     midiTick;
        uint16_t    midiOutTick;


        uint32_t    samplesSinceLastTick;
        uint32_t    averageSamplesPerTick;

        void calculateDivision (void) {
            // calculation for division using samples per bar, and then using the division from there.
            samplesPerDivision = ((((60 * sampleRate) << 2) /_bpm) / clockDivision);
        }
        void calculatePulse (void) {
            samplesPerPulse = (((60 * sampleRate) << 2) /_bpm) / 96;
        }
    }

    void        init (void);
    void        setSampleRate (uint16_t sample_rate = SAMPLE_RATE); // provides a default sample_rate linked to the global sample rate... this could be useful for sample rate updatings in the future.
    void        setBPM (uint16_t bpm);
    uint16_t    getBPM (void);
    void        setDivision (uint8_t division);

    void        internalClockTick (void);
    uint32_t    getClockTick(void);
    uint32_t    getSamplesPerDivision(void);
    void        update (void);
    
    void        setClockChanged(bool changed);
    bool        getClockChanged (void);
    void        resetClockChanged (void);
    uint8_t     getBeat (void);

    void        midiClockTick (void);
    void        startMidiClock (void);
    void        stopMidiClock (void);
    void        midiClockPosition (uint16_t input);
    void        checkMidiClock (void);
    bool        isMidiClockPresent(void);

}

