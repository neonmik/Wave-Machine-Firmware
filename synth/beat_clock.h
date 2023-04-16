#pragma once    

#include "pico/stdlib.h"

namespace BEAT_CLOCK {


    namespace {
        uint32_t _tick = 0;

        uint8_t _max = 8; // 4/4 - 4 beats in a bar
        uint8_t _beat = 0;

        bool _changed;
        uint32_t _samples_per_division;

        uint32_t _samples_rate;
        uint16_t _bpm = 120;
        uint8_t _division = 8;

        long map(long x, long in_min, long in_max, long out_min, long out_max) {
            volatile long temp = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
            return temp;
        }
    }

    void init (uint8_t bpm, uint16_t sample_rate);
    void set_samplerate (uint16_t sample_rate);
    void set_bpm (uint16_t bpm);
    uint8_t get_bpm ();
    void set_division (uint16_t division);

    void tick (void);
    void update (void);
    
    void set_changed(bool changed);
    bool get_changed (void);
    uint8_t get_beat (void);

   

}

