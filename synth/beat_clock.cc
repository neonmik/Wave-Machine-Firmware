#include "beat_clock.h"

namespace BEAT_CLOCK {

    void init (uint8_t bpm, uint16_t sample_rate) {
        _samples_rate = sample_rate;
        _bpm = bpm;
        calculate_division();
    }

    void set_samplerate (uint16_t sample_rate) {
        _samples_rate = sample_rate;
        calculate_division();
    } 

    void set_bpm (uint16_t bpm) {
        _bpm = bpm;
        calculate_division();
    }
    uint8_t get_bpm () {
        return _bpm;
    }
    void set_division (uint16_t division) {
        // set the division of the bpm clock...
        // currently /8 to get a 16th note, but rename the _samples_per_16th to _samples_per_division
        uint8_t temp = map(division, 0, 1023, 0, 9);
        switch (temp) {
            case 0: // 1/1
                _division = 1;
                break;
            case 1: // 1/2
                _division = 2;
                break;
            case 2: // 1/3
                _division = 3;
                break;
            case 3: // 1/4
                _division = 4;
                break;
            case 4: // 1/6
                _division = 6;
                break;
            case 5: // 1/8
                _division = 8;
                break;
            case 6: // 1/12 
                _division = 12;
                break;
            case 7: // 1/16
                _division = 16;
                break;
            case 8: // 1/24
                _division = 24;
                break;
            case 9: // 1/32
                _division = 32;
                break;
            // bit too much to handle...
            case 10: // 1/48    
                _division = 48;
                break;
            case 11: // 1/64
                _division = 64;
                break;
            default:
                break;
        }
        calculate_division();
    }

    void tick (void) {
        ++_tick;
        if (_tick >= _samples_per_division) { // && (_tick != _last_tick)) {
            set_changed(true);
            _tick = 0;
        }
    }
    void update (void) {
        ++_beat;
        if (_beat >= _max) {
            _beat = 0;
        }
    }

    void set_changed(bool changed) {
        _changed = changed;
    }
    bool get_changed (void) {
        return _changed;
    }
    inline uint8_t get_beat (void) {
        return _beat;
    }
   
    // void midi_tick (void) {
    //     _midi_clock_flag = true;
    //     _midi_clock_period = sample_clock - _midi_in_clock_last;
    //     _midi_in_clock_last = sample_clock;
    //     _midi_clock_tick_count++;
    //     if (_midi_clock_tick_count >= 24) {
    //         _midi_clock_tick_count = 0;
    //         // flash_led(40) ???
    //     }
    // }

    // void check_for_midi_clock (void) {
    //     if ((sample_clock - _midi_in_clock_last) > MIDI_CLOCK_TIMEOUT)) {
    //         _midi_clock_present = false;
    //     } else {
    //         _midi_clock_present = true;
    //     }
    // }
}