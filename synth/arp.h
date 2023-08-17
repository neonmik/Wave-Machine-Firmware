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
            DOWN_UP
        };

        ArpDirection _direction = UP;

        NoteState note_state = IDLE;

        bool _gap = false;

        bool  _active;
        bool  _active_last;
        
        uint8_t play_note;
        uint8_t _voice_index;


        int8_t      _play_index;
        int8_t      _count;
        int8_t      _write_index;
        bool        _notes_changed = false;

        bool        _sustain_just_released = false;

        bool        _hold;
        uint16_t    _last_hold;
        uint16_t    _last_division;
        uint16_t    _last_range;
        uint16_t    _last_direction;

        int8_t _range;
        int8_t _octave;

        bool _switch = true;
    }

    struct arp_data_t {
        uint8_t     note; // holds MIDI note number
        uint8_t     velocity;
        bool        sustained;

        void clear (void) {
            note = 0;
            velocity = 0;
            sustained = false;
        }
    };

    extern arp_data_t ARPS[MAX_ARP];

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

    void set_gap (uint16_t gap);
    void set_sustain (bool sus);

    void update_range (void);
    // void update_controls (void);

    void set_rate (uint16_t rate);
}

