#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

#include "../note_priority.h"


extern uint32_t software_index;

namespace ARP {
    namespace {
        

        uint8_t _bpm = 120;
        uint8_t max_beats = 8; // 4/4 - 4 beats in a bar
        uint16_t seq_tick = 0;
        uint16_t prev_beat;
        uint16_t beat;
        bool beat_changed;
        uint16_t _ms_per_minute = 60000;
        uint16_t _samples_per_ms;
        uint32_t _samples_per_16th;

        bool _hold;
        bool _arp_active;
        const uint8_t max_arp = 8;
        uint8_t arp_notes[max_arp];
        uint8_t buffer[max_arp];
        uint8_t arp_index;
        int8_t arp_loop;
        bool note_active;
        bool release_active;

        uint8_t _direction;
        bool _up = 1;
        bool _down = 0;

        uint16_t arp_delay = 100;
        uint16_t arp_release = 100;
        uint32_t arp_ms;
    }
    
    void on (void);
    void off (void);
    void set (bool state);
    bool get (void);
    void toggle (void);

    void tick (void);

    uint16_t samples_per_sixteenth_note (void);
    
    void set_bpm (uint8_t bpm);
    uint8_t get_bpm (void);

    void set_samplerate (uint16_t sample_rate);

    void init (uint8_t bpm, uint16_t sample_rate);
    
    void update_playback(void);
    
    void add_notes (uint8_t note);
    void clear_notes (void);
    void stop_all (void);
    
    void set_delay (uint16_t delay);
    void set_release (uint16_t release);

    void set_direction (uint8_t direction);
    
}

