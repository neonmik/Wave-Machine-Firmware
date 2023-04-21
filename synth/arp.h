#pragma once

#include <stdio.h>
#include <algorithm>
#include "pico/stdlib.h"

#include "note_priority.h"
#include "beat_clock.h"


extern uint32_t sample_clock;
extern uint32_t sample_clock_last;

#define MIDI_CLOCK_TIMEOUT 20000

namespace ARP {


    namespace {
        MAILBOX::arp_data& ARP_DATA = MAILBOX::ARP_DATA.core0;
        
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

        bool _midi_clock_flag = false;
        uint8_t _midi_start_flag;        // midi start command
        uint8_t _midi_stop_flag;    		// midi stop command
        uint32_t _midi_in_clock_last;   // stores the system time of the last received midi clock
        uint8_t _midi_clock_present;  // if a midi clock is currently present
        uint32_t _midi_clock_period;  // time in between midi clock ticks
        uint8_t _midi_clock_tick_count;

        bool& _active = MAILBOX::ARP_DATA.core0.enabled;
        bool  _active_last;
        
        const uint8_t max_arp = 8;
        uint8_t _notes[max_arp]; //all the notes stored in the arp sequence
        uint8_t _last_note;
        // uint8_t buffer[max_arp];
        
        uint16_t prev_beat;
        uint16_t beat;

        int8_t _play_index;
        int8_t _count;
        int8_t _write_index;
        bool _notes_added = false;

        
        bool _hold;
        uint16_t _last_hold;
        uint16_t _last_division;
        uint16_t _last_range;
        uint16_t _last_direction;

        int8_t _range;
        int8_t _octave;
        bool note_active = false;
        bool release_active = false;

        bool _switch = true;
    }
    

    bool get (void);


    void init (uint8_t bpm, uint16_t sample_rate);

    void set_samplerate (uint16_t sample_rate);

    void set_bpm (uint16_t bpm);
    uint8_t get_bpm (void);
    
    void update(void);
    
    void add_notes (uint8_t note);
    void remove_notes (uint8_t note);
    void organise_notes (void);
    void clear_notes (void);
    void pass_notes(void);
    void grab_notes(void);
    void stop_all (void);
    
    void set_state (bool state);
    void set_hold (uint16_t hold);
    void set_division (uint16_t division);
    void set_direction (uint16_t direction);
    void set_range (uint16_t range);

    void update_range (void);
    void update_controls (void);

    void set_rate (uint16_t rate);
    long map (long x, long in_min, long in_max, long out_min, long out_max);
    
}

