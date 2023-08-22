#pragma once    

#include "pico/stdlib.h"

#include "../config.h"
#include "../functions.h"

#define MIDI_CLOCK_TIMEOUT 670000 // time in us for 1 pulse at 24ppqn



namespace CLOCK {

    namespace {



        uint32_t _tick = 0;
        uint32_t _tock = 0;
        uint32_t _last_tick = 0;

        uint8_t _max = 8; // 4/4 - 4 beats in a bar
        uint8_t _beat = 0;

        bool _changed;
        uint32_t _samples_per_division;

        uint32_t _sample_rate;
        uint16_t _bpm = 120;
        uint8_t  _beats = 4;
        uint8_t _division = 8;

        bool _midi_clock_present = false;
        uint8_t midi_division = 24;

        uint8_t _midi_start_flag;        // midi start command
        uint8_t _midi_stop_flag;    		// midi stop command
        uint32_t _midi_in_clock_last;   // stores the system time of the last received midi clock
        uint32_t _midi_clock_period;  // time in between midi clock ticks
        uint8_t _midi_clock_tick_count;

        uint32_t _delta;
        bool _delta_flag = 0;

        void calculate_division (void) {
            // calculation for division using samples per bar, and then using the division from there.
            _samples_per_division = (((60 * _sample_rate) / (_bpm / _beats)) / _division);


        }
    }

    void Init (void);
    void set_samplerate (uint16_t sample_rate = SAMPLE_RATE); // provides a default sample_rate linked to the global sample rate... this could be useful for sample rate updatings in the future.
    void setBpm (uint16_t bpm);
    uint8_t get_bpm ();
    void setDivision (uint8_t division);

    void tick (void);
    void Update (void);
    
    void set_changed(bool changed);
    bool get_changed (void);
    uint8_t get_beat (void);

    void midi_tick (void);
    void start_midi_clock (void);
    void stop_midi_clock (void);
    void check_for_midi_clock (void);

}

