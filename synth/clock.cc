#include "clock.h"

namespace CLOCK {

    void Init () {
        _sample_rate = SAMPLE_RATE;
        _bpm = DEFAULT_BPM;
        calculate_division();
    }

    void set_samplerate (uint16_t sample_rate) {
        _sample_rate = sample_rate;
        calculate_division();
    } 

    void setBpm (uint16_t bpm) {
        _bpm = bpm;
        calculate_division();
    }
    uint8_t get_bpm () {
        return _bpm;
    }
    void setDivision (uint8_t division) {
        // set the division of the bpm/midi clock
        switch (division) {
            case 0: // Whole Note (1/1)
                _division = 1;
                midi_division = 96;
                break;
            case 1: // Half Note (1/2)
                _division = 2;
                midi_division = 48;
                break;
            case 2: // Half note Triplet (1/3)
                _division = 3;
                midi_division = 32;
                break;
            case 3: // Quarter Note (1/4)
                _division = 4;
                midi_division = 24;
                break;
            case 4: // Quarter Note Triplet (1/6)
                _division = 6;
                midi_division = 16;
                break;
            case 5: // 8th Note (1/8)
                _division = 8;
                midi_division = 12;
                break;
            case 6: // 8th Note Triplet (1/12)
                _division = 12;
                midi_division = 8;
                break;
            case 7: // 16th Note (1/16)
                _division = 16;
                midi_division = 6;
                break;
            case 8: // 16th Note Triplet (1/24)
                _division = 24;
                midi_division = 4;
                break;
            case 9: // 32nd Note (1/32)
                _division = 32;
                midi_division = 3;
                break;
            case 10: // 32nd Note Triplet (1/48)
                _division = 48;
                midi_division = 2;
                break;
            // not used yet... 
            case 11: // 1/64 - Midi can't handle this, and I've not missed it. Could be possible if extrapolate the single midi tick?
                _division = 64;
                midi_division = 1.5;
                break;
            default:
                break;
        }
        calculate_division();
    }

    void tick (void) {
        ++_tick;
        if (_tick >= _samples_per_division) {
            if (!_midi_clock_present) set_changed(true);
            _tick = 0;
        }
    }

    void check_sample_clock (void) {
        
    }
    void Update (void) {
        // check_sample_clock();
        check_for_midi_clock();
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
   
    void midi_tick (void) {
        // set current time in µs
        uint32_t _current_time = to_us_since_boot(get_absolute_time()); 
        // raise the flag to make it known we are now reciveing midi clock
        _midi_clock_present = true; 
        // set the current time in µs for use in checking where its still here
        _midi_in_clock_last = _current_time;
        // increase the clock tick so we can raise a flag at the correct divisions for 24ppqn
        ++_midi_clock_tick_count;
        if (_midi_clock_tick_count >= midi_division) {
            set_changed(true);
            // printf("midi 1/4 \n");
            _midi_clock_tick_count = 0;
        }
    }
    
    void start_midi_clock (void) {
        _midi_clock_present = true;
    }
    void stop_midi_clock (void) {
        // maybe add some handling here to differentiate between drop out and stop. 
        _midi_clock_present =  false;
        set_changed(true); 
    }

    void check_for_midi_clock (void) {
        if (!_midi_clock_present) return; // check to see if the function still works if I use this. Just want it to not do this if it's already stopped. 
        uint32_t _current_time = to_us_since_boot(get_absolute_time()); 
        if ((_current_time - _midi_in_clock_last) > MIDI_CLOCK_TIMEOUT) {
            _midi_clock_present = false;
        }
    }

    uint8_t midi_clock_present(void) {
        return _midi_clock_present;
    }

    uint32_t get_midi_clock_period(void) {
        return _midi_clock_period;
    }
}

// ------------------------------------------------
//                  MIDI CLOCK TIMING
// ------------------------------------------------
// 
// Clock durations    (24 Pulses Per Quarter note)
// ------------------------------------------------
//                      
// Whole note           =       96  pulses
// Dotted half note     =       72  pulses 
// Half note            =       48  pulses
// Dotted quarter note  =       36  pulses 
// Quarter note         =       24  pulses (24ppqn)
// 8th note             =       12  pulses
// Triplet 8th note     =       8   pulses
// 16th note            =       6   pulses
// Triplet 16th note    =       4   pulses
// 32nd note            =       3   pulses
// Triplet 32nd note    =       2   pulses
// 64th note            =       1.5 pulses
// Triplet 64th note    =       1   pulses
// 
// ------------------------------------------------
// ------------------------------------------------


