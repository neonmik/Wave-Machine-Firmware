#include "arp.h"

namespace ARP {

    void on (void) {
        _active = true;
        clear_notes();
    }

    void off (void) {
        _active = false;
    }

    void set (bool state) {
        _active = state;
        clear_notes();
        stop_all();
    }

    bool get (void) {
        return _active;
    }

    void toggle (void) {
        _active = !_active;
        clear_notes();
        stop_all();
    }

    void set_bpm (uint8_t bpm) {
        _bpm = bpm;
        _samples_per_16th = _samples_per_ms * (_ms_per_minute/_bpm)/8;
    }
    void set_rate (uint16_t rate) {
        set_bpm(rate>>2);
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

    void set_hold (uint16_t hold) {
        bool temp = (bool)(hold>>9);
        if (temp != _hold) clear_notes();
        _hold = temp;
    }


    void arpeggiate(ArpDirection direction) {
        switch (direction) {
            case UP:
                _play_index++;
                if (_play_index >= _count) _play_index = 0;
                break;
            case DOWN:
                _play_index--;
                if (_count > 1) {
                    if (_play_index <= -1) _play_index = _count - 1;
                } else {
                    _play_index = 0;
                }
                break;
            case UP_DOWN:
                if (_switch) {
                    _play_index++;
                    if (_play_index >= _count) {
                        _play_index = _count > 1 ? _count - 2 : 0;
                        _switch = false;
                    }
                } else {
                    _play_index--;
                    if (_play_index < 0) {
                        _play_index = _count > 1 ? 1 : 0;
                        _switch = true;
                    }
                }
                break;
            case DOWN_UP:
                if (_switch) {
                    _play_index--;
                    if (_play_index < 0) {
                        _play_index = _count > 1 ? 1 : 0;
                        _switch = false;
                    }
                } else {
                    _play_index++;
                    if (_play_index >= _count) {
                        _play_index = _count > 1 ? _count - 2 : 0;
                        _switch = true;
                    }
                }
                break;
        }
    }

    void update_playback(void) {
        if (_active) {
            if ((sample_clock >= _samples_per_16th)) { //} && (sample_clock != sample_clock_last)) {
                sample_clock = 0;
                beat++;
                beat_changed = true;
            }

            if (beat_changed) {
                switch (note_state) {
                    case IDLE:
                        if (_play_index >= _count) {
                            _play_index = 0;
                        }
                        if (_notes[_play_index]) {
                            Note_Priority::priority(0x90, _notes[_play_index], 127);
                            _last_note = _notes[_play_index];
                            note_state = NOTE_ACTIVE;
                        }
                        break;
                    case NOTE_ACTIVE:
                        Note_Priority::priority(0x80, _last_note, 0);
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
        for (int i = 0; i < max_arp; ++i) {
            if (_notes[i] == note) {
                return;
            }
        }

        // played order
        _notes[_write_index] = note;
        ++_count;
        ++_write_index;
        if (_count >= max_arp) {
            _count = max_arp;
        }
        if (_write_index >= max_arp) {
            _write_index = 0;
        }

        // organises the order to ascending, just comment if you want played order
        int j;
        for (j = _count - 1; j > 0 && _notes[j - 1] > note; j--) {
            _notes[j] = _notes[j - 1];
        }
        _notes[j] = note;
        
    }

    void remove_notes (uint8_t note) {
        if (_hold) {
            return;
        }
        if (_count == 0) {
            return;
        }
        for (int i = 0; i <= _count; ++i) {
            if (_notes[i] == note) {
                // Shift all the notes after the removed note back by one
                for (int j = i; j < _count + 1; ++j) {
                    _notes[j] = _notes[j + 1];
                    _notes[j + 1] = 0;
                    }
                // Decrement arp_count
                --_count;
                --_write_index;
                if (_count < 0) _count = 0;
                if (_write_index <= 0) _write_index = _count;

                //don't return here in case the note is in the list more than once
            }
        }
    }

    void clear_notes (void) {
        for (int i = 0; i < max_arp; i++) {
            _notes[i] = 0;
        }
        _write_index = 0;
        _count = 0;
    }

    void stop_all (void) {
        Note_Priority::voice_clear();
    }

    void set_delay (uint16_t delay) {
        arp_delay = delay;
    }

    void set_release (uint16_t release) {
        arp_release = release;
    }
    void set_direction (uint16_t value) {
        switch (value>>8) {
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