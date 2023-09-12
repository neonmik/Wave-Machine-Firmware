#pragma once

#include "pico/stdlib.h"

#include "../queue.h"
#include "../config.h"

#include "arp.h"




extern uint32_t sample_clock;


namespace NOTE_HANDLING {


    enum class Priority {
        FIRST,      // First/oldest notes stay the longest
        LAST,       // Last/newest notes stay the longest
        HIGHEST,    // Highest notes stay the longest
        LOWEST    ,  // Lowest notes stay the longest
    };

    enum class Mode {
        MONO,
        PARA,
    };

    namespace {
        Priority    _priority = Priority::LAST;

        uint8_t     _notes_on;
        
        bool        _sustain;
        bool        _sustain_just_released;

        Mode        _mode  = Mode::MONO;

        int8_t     _voices_active = 0;
        bool       _filter_active = false;


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
                activation_time = sample_clock;
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
    void voice_on(uint8_t slot, uint8_t note, uint8_t velocity);
    void voice_off(uint8_t slot, uint8_t note, uint8_t velocity);
    bool voices_check (uint8_t slot);
    uint8_t voices_get (uint8_t slot);
    void voices_stop (void);
    void voice_stop (uint8_t note);
    void voices_stop_all (void);
    void voices_panic (void);

    // Note priority detection
    void priority(uint8_t status, uint8_t note, uint8_t velocity);
    void release(uint8_t note, uint8_t velocity);

    void Update (void);

    void check_release (void);

    void note_on (uint8_t note, uint8_t velocity);
    void note_off (uint8_t note, uint8_t velocity);
    void notes_clear (void);
    uint8_t get_notes_on(void);

    void sustain_pedal (uint16_t status);
    bool getSustain (void);

    void setPriority (uint16_t input);

}

