#include "arp.h"

namespace ARP {

    arp_data_t ARPS[MAX_ARP];
    
    void init () {
        // CLOCK::init();
    }
    void set_state (bool state) {
        if (state != _active) {
            _active = state;
            if (!_active) {
                // passes notes following deactivation the arp, only if you're holding them down so they don't hold forever.
                if (!_hold) pass_notes();
                else stop_all();
                clear_all_notes();
            }
            if (_active) {
                stop_all();
                grab_notes();
            }   
        }
    }
    bool get_state (void) {
        return _active;
    }

    void reset () {
        if (!_active) {
            // passes notes following deactivation the arp, only if you're holding them down so they don't hold forever.
            if (!_hold) pass_notes();
            else stop_all();
            clear_all_notes();
        }
        if (_active) {
            stop_all();
            grab_notes();
        }   
    }

    void arpeggiate(ArpDirection direction) {
        switch (direction) {
            case UP:
                ++_play_index;
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
                    ++_play_index;
                    if (_play_index >= _count) {
                        _play_index = _count > 1 ? _count - 2 : 0;
                        _switch = false;
                    }
                } else {
                    --_play_index;
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
                    ++_play_index;
                    if (_play_index >= _count) {
                        _play_index = _count > 1 ? _count - 2 : 0;
                        _switch = true;
                        update_range();
                    }
                }
                break;
        }
    }
    void update_range () {
        // add variable for direction for new arp modes (more similar in handling to JUNO)
        ++_octave;
        if (_octave > _range) _octave = 0;
    }

    void update (void) {
        CLOCK::update();
        if (_active) {
            if (_sustain_just_released) {
                clear_all_notes();

                // check all notes
                // for (int i = 0; i < MAX_ARP; i++) {
                //     // if its currently being sustained
                //     if (ARPS[i].sustained) {
                //         ARPS[i].clear();
                //         --_count;
                //         --_write_index;
                //         NOTE_HANDLING::voices_dec();
                //         if (_count < 0) _count = 0; // maybe add a line here to reset range and play index? 
                //         if (_write_index <= 0) _write_index = _count;
                //         _notes_changed = true;
                //     }
                // }
                _sustain_just_released = false;
            }
            organise_notes();
            if (CLOCK::get_changed()) {
                switch (note_state) {
                    case NOTE_ACTIVE:
                        // old voice handling, keeping until fully bug tested
                        // NOTE_HANDLING::priority(0x80, play_note, 0); 

                        // ----------------------
                        // New Arp Voice handling 
                        NOTE_HANDLING::voice_off(_voice_index, play_note, 0);
                        _voice_index++;
                        if (_voice_index >= POLYPHONY) _voice_index = 0;
                        // ----------------------

                        arpeggiate(_direction);
                        note_state = IDLE;
                        if (_gap) break; // comment to remove gap between notes (goes stright into next switch function instead of waiting)
                    case IDLE:
                        if (_play_index >= _count) {
                            _play_index = 0;
                        }
                        if (ARPS[_play_index].note) {
                            play_note = ((ARPS[_play_index].note)+(_octave*12));

                            NOTE_HANDLING::voice_on(_voice_index, play_note, 127);

                            note_state = NOTE_ACTIVE;
                        }
                        break;
                }
                CLOCK::set_changed(false);
            }
        }
    }

 
    void add_note (uint8_t note) {
        for (int i = 0; i < MAX_ARP; i++) { // change for count?
            if (ARPS[i].note == note) {
                _notes_changed = true;
                return;
            }
        }
        //    
        // played order
        ARPS[_write_index].note = note;
        ++_count;
        ++_write_index;
        NOTE_HANDLING::voices_inc();
        if (_count >= MAX_ARP) {
            _count = MAX_ARP;
        }
        if (_write_index >= MAX_ARP) {
            _write_index = 0;
        }
        _notes_changed = true;
    }
    void remove_note (uint8_t note) {
        if (_hold) {
            for (int i = 0; i <= _count; i++) {
                if (ARPS[i].note == note) {
                    ARPS[i].sustained = true;
                    break; 
                }
            }
            _notes_changed = true;
            return;
        }
        if (_count == 0) {
            return;
        }
        for (int i = 0; i <= _count; i++) {
            if (ARPS[i].note == note) {
                // Shift all the notes after the removed note back by one
                for (int j = i; j < _count; j++) {
                    ARPS[j].clear();
                    ARPS[j] = ARPS[j + 1];
                    ARPS[j + 1].clear();
                }
                // Decrement _count
                --_count;
                --_write_index;
                NOTE_HANDLING::voices_dec();
                if (_count < 0) _count = 0; // maybe add a line here to reset range and play index? 
                if (_write_index <= 0) _write_index = _count;
                _notes_changed = true;
                //don't return here in case the note is in the list more than once
            }
        }
    }
    // void clear_note (uint8_t slot) {
    //     ARPS[slot].clear();
    //     --_count;
    //     --_write_index;
    //     NOTE_HANDLING::voices_dec();
    //     if (_count < 0) _count = 0; // maybe add a line here to reset range and play index? 
    //     if (_write_index <= 0) _write_index = _count;
    //     _notes_changed = true;
    // }
    void organise_notes () {
        if (_notes_changed) {
            // printf("notes in:  %d | %d | %d | %d | %d | %d | %d | %d \n", ARPS[0].note, ARPS[1].note, ARPS[2].note, ARPS[3].note, ARPS[4].note, ARPS[5].note, ARPS[6].note, ARPS[7].note);
            uint8_t length      = _count;   // how many entries in the array
            arp_data_t temp;        // set a temp value, never going to be more than 127, so uint8_t is fine
            for (int i = 0; i < length; i++) {     
                for (int j = i+1; j < length; j++) {     
                    if(ARPS[i].note > ARPS[j].note) {    
                        temp = ARPS[i];    
                        ARPS[i] = ARPS[j];    
                        ARPS[j] = temp;    
                    }     
                }
            }
            _notes_changed = false;
            // printf("notes out: %d | %d | %d | %d | %d | %d | %d | %d \n", ARPS[0].note, ARPS[1].note, ARPS[2].note, ARPS[3].note, ARPS[4].note, ARPS[5].note, ARPS[6].note, ARPS[7].note);
        }
    }

    void clear_all_notes () {
        for (int i = 0; i < MAX_ARP; i++) {
            ARPS[i].clear();
        }
        _write_index = 0;
        _count = 0;
        _octave = 0;
        _play_index = 0;
        NOTE_HANDLING::voices_clr();
    }
    
    // functions for start/stop of arp
    void pass_notes () {
        NOTE_HANDLING::voices_set(_count);
        for (int i = 0; i < MAX_ARP; i++) {
            NOTE_HANDLING::voice_on(i, ARPS[i].note, 127);
        }
    }
    void grab_notes () {
        for (int i = 0; i < MAX_ARP; i++) {
            add_note(NOTE_HANDLING::voices_get(i));
        }
        organise_notes();
    }
    void stop_all () {
        NOTE_HANDLING::voices_clr();
        NOTE_HANDLING::voices_stop();
    }
    
    void set_hold (uint16_t hold) {
        // if (hold == _last_hold) return;
        bool temp = (bool)(hold>>9);
        if (_hold != temp) {
            _hold = temp;
            if (!_hold) {
                // Only clears the notes if hold has been disengaged - lets you play notes then engage whatevers being held
                clear_all_notes();
            }
        }
    }
    void set_division (uint16_t division) {
        if (division == _last_division) return;
        _last_division = division;
        CLOCK::set_division(division);
    }
    void set_range (uint16_t range) {
        if (range == _last_range) return;
        _last_range = range;
        // bit shift to get 0-4 octaves of range for the Arp
        _range = range>>8;

        // optional unquantized range changing:
        // if (_octave > _range) _octave = _range; // if you change range while playing it will pull it back immediately, instead of waiting till next range check
    }   
    void set_direction (uint16_t direction) {
        if (direction == _last_direction) return;
        _last_direction = direction;
        // bitshift to get 0-3 for the Arp direction
        switch (direction>>8) {
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
    
    void set_gap (uint16_t gap) {
        bool temp = (bool)(gap>>9);
        if (_gap != temp) _gap = temp;
    }
    void set_sustain (bool sus) {
        if (_hold != sus) {
            _hold = sus;
            if (!_hold) {
                // if sustain has been released...
                // raise flag to clear notes out?
                _sustain_just_released = true;
            } 
        }
    }

    void set_bpm (uint16_t bpm) {
        CLOCK::set_bpm(map(bpm, KNOB_MIN, KNOB_MAX, 30, 350));
    }
}