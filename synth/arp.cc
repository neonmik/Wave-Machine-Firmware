#include "arp.h"

namespace ARP {

    void on (void) {
        _arp_active = true;
        clear_notes();
    }

    void off (void) {
        _arp_active = false;
    }

    void set (bool state) {
        _arp_active = state;
        clear_notes();
        stop_all();
    }

    bool get (void) {
        return _arp_active;
    }

    void toggle (void) {
        _arp_active = !_arp_active;
        clear_notes();

    }

    void set_bpm (uint8_t bpm) {
        _bpm = bpm;
        _samples_per_16th = _samples_per_ms * (_ms_per_minute/_bpm)/8;
    }
    uint8_t get_bpm () {
        return _bpm;
    }

    void set_samplerate (uint16_t sample_rate) {
        _samples_per_ms = (sample_rate / 1000);
    } 

    void init (uint8_t bpm, uint16_t sample_rate) {
        set_samplerate(sample_rate);
        set_bpm(bpm);
    }

    void up () {
        arp_index++; // pushes the arp to the next note
        if (arp_index >= arp_count) arp_index = 0; // if the arp has played all the notes that are active on the keyboard, it resets to the first note of the arp
    }

    void down () {
        arp_index--; // pushes the arp to the next note
        if (arp_count > 1) {
            if (arp_index <= -1) arp_index = arp_count - 1; // if the arp has played all the notes that are active on the keyboard, it resets to the first note of the arp
        } 
        else {
            arp_index = 0;
        }
    }

    void update_playback(void) {
        if (_arp_active) {
            if (software_index >= _samples_per_16th) {
                software_index = 0;
                beat++;
                beat_changed = true;
            }

            if (beat_changed) {
                if (beat >= max_beats) beat = 0;

                if (note_active && !release_active) {
                    
                    Note_Priority::note_off(arp_index, arp_notes[arp_index], 0);

                    release_active = true;
                }

                if (arp_notes[arp_index] && !note_active) {
                    Note_Priority::note_on(arp_index, arp_notes[arp_index], 127);

                    release_active = false;
                    note_active = true;
                    
                }

                if (note_active && release_active) {
                    note_active = false;
                    // stop_all();

                    down();

                }

                beat_changed = false;
                // prev_beat = beat; 
            }
        }
    }


    void add_notes (uint8_t note) {
        arp_notes[arp_loop] = note;
        ++arp_count;
        ++arp_loop;
        if (arp_count> max_arp-1) {
            arp_loop = 0;
            arp_count = max_arp;
        }
    }

    void clear_notes (void) {
        if (!_hold) {
            for (int i = 0; i < max_arp; i++) {
                arp_notes[i] = 0;
            }
            arp_loop = 0;
            arp_count = 0;
        }
    }

    void stop_all (void) {
        for (int i = 0; i < max_arp; i++) {
            // if (arp_notes[i] == 0) {
                Note_Priority::note_off(i, 0, 0);
            }
        // }
    }

    void set_delay (uint16_t delay) {
        arp_delay = delay;
    }

    void set_release (uint16_t release) {
        arp_release = release;
    }

    void set_direction (uint8_t direction) {
        _direction = direction;
    }
}