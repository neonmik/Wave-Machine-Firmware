#pragma once    

#include "pico/stdlib.h"

namespace BEAT_CLOCK {


    namespace {
        uint32_t _tick = 0;
        uint32_t _last_tick = 0;

        uint8_t _max = 8; // 4/4 - 4 beats in a bar
        uint8_t _beat = 0;

        bool _changed;
        uint32_t _samples_per_division;

        uint32_t _samples_rate;
        uint16_t _bpm = 120;
        uint8_t _division = 8;

        // bool _midi_clock_flag = false;
        // uint8_t _midi_start_flag;        // midi start command
        // uint8_t _midi_stop_flag;    		// midi stop command
        // uint32_t _midi_in_clock_last;   // stores the system time of the last received midi clock
        // uint8_t _midi_clock_present;  // if a midi clock is currently present
        // uint32_t _midi_clock_period;  // time in between midi clock ticks
        // uint8_t _midi_clock_tick_count;

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

