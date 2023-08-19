#pragma once

// #include <stdio.h>
// #include <algorithm>
#include "pico/stdlib.h"

#include "../functions.h"
#include "../config.h"

#include "note_handling.h"
#include "clock.h"

namespace ARP {


    namespace {
        
        enum NoteState {
            IDLE,
            NOTE_ACTIVE,
            RELEASE_ACTIVE
        };

        enum ArpDirection {
            UP,
            DOWN,
            UP_DOWN,
            DOWN_UP,
            // RAND,
            // PLAY_ORDER
        };

        ArpDirection arpDirection = UP;



        NoteState note_state = IDLE;

        bool isRestEnabled = false;

        bool  isArpActive;
        
        uint8_t     currentPlayNote;
        uint8_t     currentVoiceIndex;
        int8_t      currentPlayIndex;


        int8_t      currentNoteCount;
        int8_t      _write_index;
        bool        _notes_changed = false;

        bool        isSustainJustReleased = false;

        bool        isHoldEnabled;
        uint16_t    _last_hold;
        uint16_t    _last_division;
        uint16_t    _last_range;
        uint16_t    _last_direction;

        int8_t _range;
        int8_t currentOctave;

        bool _switch = true;
    }

    struct ArpData {
        uint8_t     note; // holds MIDI note number
        uint8_t     velocity;
        bool        sustained;

        void add (uint8_t input) {
            note = input;
        }
        void sustain (void) {
            sustained = true;
        }
        void clear (void) {
            note = 0;
            velocity = 0;
            sustained = false;
        }
        bool isActive (void) {
            return note;
        }
        uint8_t play (void) {
            return note;
        }
    };

    extern ArpData arpVoices[MAX_ARP];

    void init (void);
    
    void update(void);
    
    void add_note (uint8_t note);
    void remove_note (uint8_t note);
    void clear_note (uint8_t slot);
    void organise_notes (void);
    void clear_all_notes (void);
    void pass_notes(void);
    void grab_notes(void);
    void stop_all (void);
    
    void set_state (bool state);
    bool get_state (void);
    void reset (void);

    void set_hold (uint16_t hold);
    void set_division (uint16_t division);
    void set_range (uint16_t range);
    void set_direction (uint16_t direction);

    void set_rest (uint16_t gap);
    void set_sustain (bool sus);

    void updateOctave (bool rising);
    // void update_controls (void);

    void set_bpm (uint16_t bpm);
}

