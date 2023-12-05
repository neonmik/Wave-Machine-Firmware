#include "clock.h"

#include "../midi.h"
namespace CLOCK {

    void init () {
        sampleRate = SAMPLE_RATE;
        _bpm = DEFAULT_BPM;
        calculateDivision();
    }

    void setSampleRate (uint16_t sample_rate) {
        sampleRate = sample_rate;
        calculateDivision();
    } 

    void setBPM (uint16_t bpm) {
        _bpm = bpm;
        calculateDivision();
        calculatePulse();
    }
    uint8_t getBPM () {
        return _bpm;
    }
    void setDivision (uint8_t division) {
        // set the division of the bpm/midi clock
        switch (division) {
            case 0: // Whole Note (1/1)
                clockDivision = 1;
                midiDivision = 96;
                break;
            case 1: // Half Note (1/2)
                clockDivision = 2;
                midiDivision = 48;
                break;
            case 2: // Half note Triplet (1/3)
                clockDivision = 3;
                midiDivision = 32;
                break;
            case 3: // Quarter Note (1/4)
                clockDivision = 4;
                midiDivision = 24;
                break;
            case 4: // Quarter Note Triplet (1/6)
                clockDivision = 6;
                midiDivision = 16;
                break;
            case 5: // 8th Note (1/8)
                clockDivision = 8;
                midiDivision = 12;
                break;
            case 6: // 8th Note Triplet (1/12)
                clockDivision = 12;
                midiDivision = 8;
                break;
            case 7: // 16th Note (1/16)
                clockDivision = 16;
                midiDivision = 6;
                break;
            case 8: // 16th Note Triplet (1/24)
                clockDivision = 24;
                midiDivision = 4;
                break;
            case 9: // 32nd Note (1/32)
                clockDivision = 32;
                midiDivision = 3;
                break;
            case 10: // 32nd Note Triplet (1/48)
                clockDivision = 48;
                midiDivision = 2;
                break;
            // not used yet... 
            case 11: // 64th Note - Midi can't handle this, and I've not missed it. Could be possible if extrapolate the single midiTick?
                clockDivision = 64;
                midiDivision = 1.5;
                break;
            case 12: // 64th Note Triplet
                clockDivision = 84;
                midiDivision = 1;
                break;
            default:
                break;
        }
        calculateDivision();
    }

    void internalClockTick (void) {
        ++clockTick;

        if (clockTick >= samplesPerDivision) {
            if (!midiClockPreset) setClockChanged(true);
            clockTick = 0;
        }

        ++midiOutTick;

        if (midiOutTick >= samplesPerPulse) {
            if (!midiClockPreset) MIDI::toggleClockFlag();
            
            midiOutTick = 0;
        }
    }

    uint32_t getClockTick (void) {
        return clockTick;
    }
    uint32_t getSamplesPerDivision(void) {
        return samplesPerDivision;
    }

    void update (void) {
        checkMidiClock();
        // printf("MIDI Clock checking: %d\n", clockTick);
    }

    void setClockChanged(bool changed) {
        clockChanged = changed;
    }
    bool getClockChanged (void) {
        if (clockChanged) {
            clockChanged = false;
            return true;
        } else {
            return false;
        }
    }
    inline uint8_t getBeat (void) {
        return _beat;
    }
   
    void midiClockTick (void) {
        // raise the flag to make it known we are now reciveing midi clock
        midiClockPreset = true; 
        
        uint32_t currentTime = sampleClock; // current time in samples
        
        midiClockPeriod = currentTime - midiClockLast;
        // printf("MIDI Clock Period: %d\n", midiClockPeriod);

        // set the current time for use in checking where its still here
        midiClockLast = currentTime;


        // increase the clock sampleClockTick so we can raise a flag at the correct divisions for 24ppqn
        ++midiTick;
        if (midiTick >= midiDivision) {
            setClockChanged(true);

            midiTick = 0;
        }
    }
    
    void startMidiClock (void) {
        midiClockPreset = true;
        setClockChanged(true);
    }
    void stopMidiClock (void) {
        // maybe add some handling here to differentiate between drop out and stop. 
        midiClockPreset =  false;

        // both these should only happen in AUTO mode I think
        clockTick = 0; // reset sample ticktick
        setClockChanged(true); 
    }

    void midiClockPosition (uint16_t input) {
        // 14 bit Song Position Pointer from MIDI
        // This will be used for beat sync.

        // take 16th note position and make sure that the current divison matches up with it... 
        // also add feature to make sure that the division setting is only changed on the beat.
    }

    void checkMidiClock (void) {
        if (!midiClockPreset) return;
 
        uint32_t currentTime = sampleClock;

        if ((currentTime - midiClockLast) > MIDI_CLOCK_TIMEOUT) {
            midiClockPreset = false;
        }
    }

    uint8_t isMidiClockPresent(void) {
        return midiClockPreset;
    }

    uint32_t getMidiClock(void) {
        return midiClockPeriod;
    }
}

// ------------------------------------------------
//                  MIDI CLOCK TIMING
// ------------------------------------------------
// 
// The MIDI standard uses the 24 PPQN standard, al-
// though the internal clock of the Wave Machine 
// will be more accurate than this.
// 
// 
// Clock durations    (24 Pulses Per Quarter note)
// ------------------------------------------------
//                      
// Whole note           =       96  pulses
// Dotted half note     =       72  pulses 
// Half note            =       48  pulses
// Dotted quarter note  =       36  pulses 
// Quarter note         =       24  pulses (24ppqn)
// 8th note             =       12  pulses
// Triplet 8th note     =       8   pulses
// 16th note            =       6   pulses
// Triplet 16th note    =       4   pulses
// 32nd note            =       3   pulses
// Triplet 32nd note    =       2   pulses
// 64th note            =       1.5 pulses
// Triplet 64th note    =       1   pulses
// 
// ------------------------------------------------
// ------------------------------------------------


