#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

#include "../note_priority.h"



namespace ARP {

    static uint16_t _index;

    static uint8_t _bpm = 120;
    static uint8_t max_beats = 8; // 4/4 - 4 beats in a bar
    static uint16_t seq_tick = 0;
    static uint16_t prev_beat;
    static uint16_t beat;
    static bool beat_changed;
    static uint16_t _s_p_ms;
    static uint32_t _s_p_sixteenth;

    static bool _hold;
    static bool _arp_active;
    static const uint8_t max_arp = 8;
    static uint8_t arp_notes[max_arp];
    static uint8_t buffer[max_arp];
    static uint8_t arp_index;
    static int8_t arp_loop;
    static bool note_active;
    static bool release_active;

    static uint8_t _direction;
    static bool _up = 1;
    static bool _down = 0;

    static uint16_t arp_delay = 100;
    static uint16_t arp_release = 100;
    static uint32_t arp_ms;

    
    void on (void);
    void off (void);
    void set (bool state);
    bool get (void);
    void toggle (void);

    void index (void);

    uint16_t samples_per_sixteenth_note (void);
    
    void set_bpm (uint8_t bpm);
    uint8_t get_bpm (void);

    void set_samplerate (uint16_t samplerate);

    void init (uint8_t bpm, uint16_t samplerate);
    
    void update_playback(void);
    
    void add_notes (uint8_t note);
    void clear_notes (void);
    void stop_all (void);
    
    void set_delay (uint16_t delay);
    void set_release (uint16_t release);

    void set_direction (uint8_t direction);
    
}

