#pragma once

#include "../config.h"

#include "../queue.h"

#include "arp.h"




extern uint32_t sampleClock;


namespace NOTE_HANDLING {


    enum class Priority {
        FIRST,      // First/oldest notes stay the longest
        LAST,       // Last/newest notes stay the longest
        HIGHEST,    // Highest notes stay the longest
        LOWEST    ,  // Lowest notes stay the longest
    };

    struct Note {
        uint8_t     note;
        uint8_t     velocity;
        bool        sustained;
    };


    namespace {
        Priority    _priority = Priority::LAST;

        Note noteState[128];

        uint8_t     _notes_on;
        
        bool        sustainPedal;
        bool        isSustainJustReleased;

        bool       filterActive = false;


        struct VoiceData {
            uint8_t     note; // holds MIDI note number
            uint8_t     velocity;
            bool        gate; // tracks whether note is being physcially played
            bool        active; // tracks whether the note is still sounding out
            uint32_t    activation_time; // time when the note was activated
            bool        sustained;

            void on (uint8_t _note, uint8_t _velocity) {
                note = _note;
                velocity = _velocity;
                gate = true;
                active = true;
                activation_time = sampleClock;
                sustained = false; // think this needs to be outside of this call so it isnt used by the ARP.
            }
            void off (void) {
                gate = false;
            }
            void clear (void) {
                note = 0;
                velocity = 0;
                active = false;
                activation_time = 0;
                sustained = false;
            }
        };
    }
    
    extern VoiceData VOICES[POLYPHONY];
    // actual synth voice notes, also add MIDI out here
    void voiceOn(uint8_t slot, uint8_t note, uint8_t velocity);
    void voiceOff(uint8_t slot, uint8_t note, uint8_t velocity);
    bool voicesCheckSustain (uint8_t slot);
    uint8_t voicesActiveNote (uint8_t slot);
    void voiceStop (uint8_t note);

    void voicesStop (void);
    void voicesPanic (void);

    // Note priority detection
    void priority(uint8_t note, uint8_t velocity);
    void release(uint8_t note, uint8_t velocity);

    void update (void);

    void checkReleaseMessages (void);

    void noteOn (uint8_t note, uint8_t velocity);
    void noteOff (uint8_t note, uint8_t velocity);
    // void notes_clear (void);
    // uint8_t get_notes_on(void);

    void setSustainPedal (uint16_t status);
    bool getSustainPedal (void);

    void setPriority (uint16_t input);

}

