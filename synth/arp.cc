#include "arp.h"

namespace ARP {

    // void on (void) {
    //     _active = true;
    //     set_state(_active);

    // }

    // void off (void) {
    //     _active = false;
    //     set_state(_active);
    // }

    // void toggle (void) {
    //     _active = !_active;
    //     clear_notes();
    //     stop_all();
    // }

    void init (uint8_t bpm, uint16_t sample_rate) {
        BEAT_CLOCK::init(bpm, sample_rate);
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
        return _active;
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
    void update_range () {
        ++_octave;
        if (_octave > _range) _octave = 0;
    }

    void update (void) {
        if (_active) {
            set_hold(ARP_DATA.hold);
            set_division(ARP_DATA.division);
            set_range(ARP_DATA.range);
            set_direction(ARP_DATA.direction);
            
            BEAT_CLOCK::update();

            if (BEAT_CLOCK::get_changed()) {
                switch (note_state) {
                    case NOTE_ACTIVE:
                        NOTE_PRIORITY::priority(0x80, _last_note, 0);
                        arpeggiate(_direction);
                        note_state = IDLE;
                        break; // comment to remove gap between notes (goes stright into next switch function instead of waiting)
                    case IDLE:
                        if (_play_index >= _count) {
                            _play_index = 0;
                        }
                        if (_notes[_play_index]) {
                            _last_note = ((_notes[_play_index])+(_octave*12));
                            NOTE_PRIORITY::priority(0x90, _last_note, 127);
                            note_state = NOTE_ACTIVE;
                        }
                        break;
                }

                

                BEAT_CLOCK::set_changed(false);
            }
        }
    }

 
    void add_notes (uint8_t note) {
        for (int i = 0; i < max_arp; ++i) {
            if (_notes[i] == note) {
                _notes_added = true;
                return;
            }
        }
        //    
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
        _notes_added = true;
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
    void organise_notes () {
        if (_notes_added) {
            uint8_t length      = _count;   // how many entries in the array
            uint8_t temp        = 0;        // set a temp value, never going to be more than 127, so uint8_t is fine

        for (int i = 0; i < length; i++) {     
            for (int j = i+1; j < length; j++) {     
                if(_notes[i] > _notes[j]) {    
                    temp = _notes[i];    
                    _notes[i] = _notes[j];    
                    _notes[j] = temp;    
                }     
            }
        }
        _notes_added = false;    
        }
    }
    void clear_notes () {
        for (int i = 0; i < max_arp; i++) {
            _notes[i] = 0;
        }
        _write_index = 0;
        _count = 0;
    }
    
    // functions for start/stop of arp
    void pass_notes () {
        for (int i = 0; i < max_arp; i++) {
            NOTE_PRIORITY::voice_on(i, _notes[i], 127);
        }
    }
    void grab_notes () {
        NOTE_PRIORITY::update();
    }
    void stop_all () {
        NOTE_PRIORITY::voices_clear();
    }
    
    void set_hold (uint16_t hold) {
        bool temp = (bool)(hold>>9);
        // Only clears the notes if hold has been disengaged - lets you play notes then engage whatevers being held
        if (_hold) {
            if (temp != _hold) clear_notes();
        }
        _hold = temp;
    }
    void set_division (uint16_t division) {
        BEAT_CLOCK::set_division(division);
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
    


    void set_rate (uint16_t rate) {
        // for freerunning/external clock
        // if (!clock_external()) {
            BEAT_CLOCK::set_bpm(map(rate, 0, 1023, 30, 350));
        // } else {
        //     break/leave alone
        // }

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