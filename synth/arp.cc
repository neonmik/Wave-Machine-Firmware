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

    void index (void) {
        if (_arp_active) {
            _index++;
            if (_index >= _s_p_sixteenth) {
                beat++;
                _index = 0;
                beat_changed = true;
            }
        }
    }
    
    uint16_t samples_per_sixteenth_note () {
        return _s_p_sixteenth;
    }
    void set_bpm (uint8_t bpm) {
        _bpm = bpm;
        // samples per ms * (ms_per_minute/bpm)/8 (to make 1/16ths note)
        _s_p_sixteenth = _s_p_ms * (60000/_bpm)/8;
    }
    uint8_t get_bpm () {
        return _bpm;
    }



    void set_samplerate (uint16_t samplerate) {
        _s_p_ms = samplerate / 1000;
    } 
    void init (uint8_t bpm, uint16_t samplerate) {
        set_samplerate(samplerate);
        set_bpm(bpm);
    }

    void update_playback(void) {

        if (_arp_active) {
            
            // if (beat == prev_beat) return;

            if (beat_changed) {
                if (beat >= arp_loop) beat = 0;

                if (!note_active) {
                    // make sure no note is playing
                    // Note_Priority::note_clear(beat);

                    Note_Priority::note_on(beat, arp_notes[beat], 127);

                    release_active = false;
                    note_active = true;

                }

                if (note_active && !release_active) {
                    
                    Note_Priority::note_off(prev_beat, arp_notes[prev_beat], 0);

                    release_active = true;
                }

                if (note_active && release_active) {
                    note_active = false;
                    // stop_all();

                }

                


                
                
                // if (arp_loop < 1) {
                //     stop_all();
                // }
                beat_changed = false;
                prev_beat = beat; 
            }

            
                // uint32_t current_ms = to_ms_since_boot(get_absolute_time());

                // if (arp_notes[arp_index] && !note_active) {
                    
                //     Note_Priority::note_clear(arp_index);

                //     Note_Priority::note_on(arp_index, arp_notes[arp_index], 127);

                //     arp_release = false;
                //     note_active = true;
                // }

                

                // if ((current_ms - arp_ms >= arp_release) && !arp_release) {
                //     Note_Priority::note_off(arp_index, arp_notes[arp_index], 0);
                //     arp_release = true;
                // }

                // if (current_ms - arp_ms >= (arp_delay+arp_release)) {
                //     arp_ms = current_ms;

                //     note_active = false;

                //     arp_index++;
                //     if (arp_index >= arp_loop) arp_index = 0;
                // }
        //     }
        }
    }

    void add_notes (uint8_t note) {
        arp_notes[arp_loop] = note;
        arp_loop++;
        if (arp_loop>(max_arp+1)) {
            arp_loop = 0;
        }
        
    }

    void clear_notes (void) {
        if (!_hold) {
            for (int i = 0; i < max_arp; i++) {
                arp_notes[i] = 0;
            }
            arp_loop = 0;
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