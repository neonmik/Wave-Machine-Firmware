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
        stop_all();
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


    void arpeggiate(ArpDirection direction) {
        switch (direction) {
            case UP:
                arp_index++;
                if (arp_index >= arp_count) arp_index = 0;
                break;
            case DOWN:
                arp_index--;
                if (arp_count > 1) {
                    if (arp_index <= -1) arp_index = arp_count - 1;
                } else {
                    arp_index = 0;
                }
                break;
            case UP_DOWN:
                if (_switch) {
                    arp_index++;
                    if (arp_index >= arp_count) {
                        arp_index = arp_count > 1 ? arp_count - 2 : 0;
                        _switch = false;
                    }
                } else {
                    arp_index--;
                    if (arp_index < 0) {
                        arp_index = arp_count > 1 ? 1 : 0;
                        _switch = true;
                    }
                }
                break;
            case DOWN_UP:
                if (_switch) {
                    arp_index--;
                    if (arp_index < 0) {
                        arp_index = arp_count > 1 ? 1 : 0;
                        _switch = false;
                    }
                } else {
                    arp_index++;
                    if (arp_index >= arp_count) {
                        arp_index = arp_count > 1 ? arp_count - 2 : 0;
                        _switch = true;
                    }
                }
                break;
        }
    }

    void update_playback(void) {
        if (_arp_active) {
            if ((sample_clock >= _samples_per_16th)) { //} && (sample_clock != sample_clock_last)) {
                sample_clock = 0;
                beat++;
                beat_changed = true;
            }

            if (beat_changed) {
                switch (note_state) {
                    case IDLE:
                        if (arp_index >= arp_count) {
                            arp_index = 0;
                        }
                        if (arp_notes[arp_index]) {
                            Note_Priority::event(0x90, arp_notes[arp_index], 127);
                            _last_note = arp_notes[arp_index];
                            note_state = NOTE_ACTIVE;
                        }
                        break;
                    case NOTE_ACTIVE:
                        Note_Priority::event(0x80, _last_note, 0);
                        note_state = RELEASE_ACTIVE;
                        break;
                    case RELEASE_ACTIVE:
                        arpeggiate(_direction);
                        note_state = IDLE;
                        break;
                }

                if (beat >= max_beats) {
                    beat = 0;
                }

                beat_changed = false;
            }
        }
    }


    void add_notes (uint8_t note) {
        arp_notes[arp_loop] = note;
        ++arp_count;
        ++arp_loop;
        if (arp_count >= max_arp) {
            arp_loop = 0;
            arp_count = max_arp;
        }
    }

    void remove_notes (uint8_t note) {
        if (arp_count == 0) {
            return;
        }
        for (int i = 0; i < arp_count; ++i) {
            if (arp_notes[i] == note) {
                // Shift all the notes after the removed note back by one
                for (int j = i; j < arp_count - 1; ++j) {
                    arp_notes[j] = arp_notes[j + 1];
                }
                // Decrement arp_count
                --arp_count;
                // If arp_loop is now past the end of the array, wrap around to 0
                if (arp_loop >= arp_count) {
                    arp_loop = 0;
                } else if (arp_loop < 0) {
                    arp_loop = arp_count - 1;
                }
                return;
            }
        }
    }

    void clear_notes (void) {
        // if (!_hold) {
            for (int i = 0; i < max_arp; i++) {
                arp_notes[i] = 0;
            }
            arp_loop = 0;
            arp_count = 0;
        // }
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
    void set_direction (uint16_t value) {
        switch (value) {
            case 0:
                _direction = ArpDirection::UP;
                break;
            case 1:
                _direction =  ArpDirection::DOWN;
                break;
            case 2:
                _direction =  ArpDirection::UP_DOWN;
                break;
            case 3:
                _direction =  ArpDirection::DOWN_UP;
                break;
        }
    }
}