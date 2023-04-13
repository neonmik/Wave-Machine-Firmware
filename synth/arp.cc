#include "arp.h"

namespace ARP {

    void on (void) {
        _active = true;
        set_state(_active);

    }

    void off (void) {
        _active = false;
        set_state(_active);
    }

    void toggle (void) {
        _active = !_active;
        clear_notes();
        stop_all();
    }

    void set_state (bool state) {
        if (state != _active) {
            _active = state;

            if (!_active) {
                // passes notes following deactivation the arp, only if you're holding them down so they don't hold forever.
                if (!_hold) pass_notes();
                clear_notes();
            }
            if (_active) {
                stop_all();
                // need to add some kind of function here to get notes that are currently held down...
                // grab_notes();
            }
            
        }
    }

    bool get (void) {
        return _active;
    }


    void set_bpm (uint16_t bpm) {
        _bpm = bpm;

        _samples_per_division = (60 * 44100 / _bpm) / _division;
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
                _play_index++;
                if (_play_index >= _count) {
                    _play_index = 0;
                    update_range();
                }
                break;
            case DOWN:
                _play_index--;
                if (_count > 1) {
                    if (_play_index <= -1) {
                        _play_index = _count - 1;
                        update_range();
                    }
                } else {
                    _play_index = 0;
                    update_range();
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
                        update_range();
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
                        update_range();
                    }
                }
                break;
        }
    }

    void update (void) {
        if (_active) {
            if ((sample_clock >= _samples_per_division)) { //} && (sample_clock != sample_clock_last)) {
                sample_clock = 0;
                beat++;
                beat_changed = true;
            }

            if (beat_changed) {
                switch (note_state) {
                    case NOTE_ACTIVE:
                        Note_Priority::priority(0x80, _last_note, 0);
                        arpeggiate(_direction);
                        note_state = IDLE;

                        // dont think this is neccessary anymore
                        // note_state = RELEASE_ACTIVE;
                        
                        break; // for gap between notes, comment to remove gap
                    case IDLE:
                        if (_play_index >= _count) {
                            _play_index = 0;
                        }
                        if (_notes[_play_index]) {
                            _last_note = ((_notes[_play_index])+(_octave*12));
                            Note_Priority::priority(0x90, _last_note, 127);
                            note_state = NOTE_ACTIVE;
                        }
                        break;

                    // dont think this is neccessary anymore - by positioning NOTE_ACTIVE before IDLE, it will always be released before the next note is played
                    // case RELEASE_ACTIVE:
                    //     arpeggiate(_direction);
                    //     note_state = IDLE;
                    //     break;
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

    void clear_notes () {
        for (int i = 0; i < max_arp; i++) {
            _notes[i] = 0;
        }
        _write_index = 0;
        _count = 0;
    }
    void pass_notes () {
        for (int i = 0; i < max_arp; i++) {
            Note_Priority::voice_on(i, _notes[i], 127);
        }
    }
    void grab_notes () {
        Note_Priority::update();
    }

    void stop_all () {
        Note_Priority::voice_clear();
    }

    // void set_delay (uint16_t delay) {
    //     arp_delay = delay;
    // }
    // void set_release (uint16_t release) {
    //     arp_release = release;
    // }
    
    void set_hold (uint16_t hold) {
        bool temp = (bool)(hold>>9);
        // Only clears the notes if hold has been disengaged - lets you play notes then engage whatevers being held
        if (_hold) {
            if (temp != _hold) clear_notes();
        }
        _hold = temp;
    }

    void set_direction (uint16_t value) {
        // bitshift to get 0-3 for the Arp direction
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
    void set_range (uint16_t value) {
        // bit shift to get 0-4 octaves of range for the Arp
        _range = value>>8;
    }
    void update_range () {
        ++_octave;
        if (_octave > _range) _octave = 0;
    }

    long map(long x, long in_min, long in_max, long out_min, long out_max) {
        volatile long temp = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        return temp;
    }

    void set_rate (uint16_t rate) {
        // for freerunning/external clock
        // if (!clock_external()) {
        _rate = map(rate, 0, 1023, 30, 350);
        set_bpm(_rate);
        // } else {
        //     break/leave alone
        // }

    }

    void set_division (uint16_t division) {
        // set the division of the bpm clock...
        // currently /8 to get a 16th note, but rename the _samples_per_16th to _samples_per_division
        uint8_t temp = map(division, 0, 1023, 0, 9);
        switch (temp)
        {
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
        // case 10: // 1/48    
        //     _division = 48;
        //     break;
        // case 11: // 1/64
        //     _division = 64;
        //     break;
        default:
            break;
        }
        _samples_per_division = (60 * 44100 / _bpm) / _division;
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