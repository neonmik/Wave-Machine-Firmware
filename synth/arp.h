#pragma once

// #include <stdio.h>
// #include <algorithm>
#include "pico/stdlib.h"

#include "../functions.h"
#include "../config.h"

#include "note_handling.h"
#include "beat_clock.h"


extern uint32_t sample_clock;
extern uint32_t sample_clock_last;

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
        
        constexpr uint8_t max_arp = 8;
        uint8_t _notes[max_arp]; //all the notes stored in the arp sequence
        uint8_t _last_note;
        uint8_t _last_index;

        int8_t      _play_index;
        int8_t      _count;
        int8_t      _write_index;
        bool        _notes_changed = false;

        
        bool        _hold;
        uint16_t    _last_hold;
        uint16_t    _last_division;
        uint16_t    _last_range;
        uint16_t    _last_direction;

        int8_t _range;
        int8_t _octave;

        bool _switch = true;
    }

    void init (void);
    
    void update(void);
    
    void add_notes (uint8_t note);
    void remove_notes (uint8_t note);
    void organise_notes (void);
    void clear_notes (void);
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

    void update_range (void);
    // void update_controls (void);

    void set_rate (uint16_t rate);
}

