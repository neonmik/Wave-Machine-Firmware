#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

extern uint16_t software_index;

namespace Arp {
    static uint8_t _bpm;
    static uint8_t max_beats = 8; // 4/4 - 4 beats in a bar
    static uint16_t seq_tick = 0;
    static uint16_t prev_beat = 1;
    static uint16_t beat;
    static uint16_t _s_p_ms;
    
    uint16_t samples_per_sixteenth_note () {
        // samples per ms * (ms_per_minute/bpm)/8 (to make 1/16ths note)
        return (_s_p_ms * (60000/_bpm)/8);
    }
    void set_bpm (uint8_t bpm) {
        _bpm = bpm;
    }
    void set_samplerate (uint16_t samplerate) {
        _s_p_ms = samplerate / 1000;
    } 
    void init (uint8_t bpm, uint16_t samplerate) {
        set_bpm(bpm);
        set_samplerate(samplerate);
    }
    void update_playback(void) {
        
        
        if (software_index & samples_per_sixteenth_note()) {
            beat++;
        }
         if (beat == max_beats) beat = 0;

        if (beat == prev_beat) return;
        prev_beat = beat;

        
    }
}

