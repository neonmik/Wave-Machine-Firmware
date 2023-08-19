#include "arp.h"

namespace ARP {

    ArpData arpVoices[MAX_ARP];
    
    void init () {
        // nothing happens here, but it might?
    }

    void set_state (bool state) {
        if (state != isArpActive) {
            isArpActive = state;
            if (!isArpActive) {
                // passes notes following deactivation the arp, only if you're holding them down so they don't hold forever.
                if (!isHoldEnabled) pass_notes();
                else stop_all();
                clear_all_notes();
            }
            if (isArpActive) {
                stop_all();
                grab_notes();
            }   
        }
    }
    bool get_state (void) {
        return isArpActive;
    }

    void reset () {
        if (!isArpActive) {
            // passes notes following deactivation the arp, only if you're holding them down so they don't hold forever.
            if (!isHoldEnabled) pass_notes();
            else stop_all();
            clear_all_notes();
        }
        if (isArpActive) {
            stop_all();
            grab_notes();
        }   
    }

    void arpeggiate(ArpDirection direction) {
        switch (direction) {
            case UP:
                ++currentPlayIndex;
                if (currentPlayIndex >= currentNoteCount) {
                    currentPlayIndex = 0;
                    update_range();
                }
                break;
            case DOWN:
                currentPlayIndex--;
                if (currentNoteCount > 1) {
                    if (currentPlayIndex <= -1) {
                        currentPlayIndex = currentNoteCount - 1;
                        update_range();
                    }
                } else {
                    currentPlayIndex = 0;
                    update_range();
                }
                break;
            case UP_DOWN:
                if (_switch) {
                    ++currentPlayIndex;
                    if (currentPlayIndex >= currentNoteCount) {
                        currentPlayIndex = currentNoteCount > 1 ? currentNoteCount - 2 : 0;
                        _switch = false;
                    }
                } else {
                    --currentPlayIndex;
                    if (currentPlayIndex < 0) {
                        currentPlayIndex = currentNoteCount > 1 ? 1 : 0;
                        _switch = true;
                        update_range();
                    }
                }
                break;
            case DOWN_UP:
                if (_switch) {
                    currentPlayIndex--;
                    if (currentPlayIndex < 0) {
                        currentPlayIndex = currentNoteCount > 1 ? 1 : 0;
                        _switch = false;
                    }
                } else {
                    ++currentPlayIndex;
                    if (currentPlayIndex >= currentNoteCount) {
                        currentPlayIndex = currentNoteCount > 1 ? currentNoteCount - 2 : 0;
                        _switch = true;
                        update_range();
                    }
                }
                break;
        }
    }
    void update_range () {
        // add variable for direction for new arp modes (more similar in handling to JUNO)
        ++currentOctave;
        if (currentOctave > _range) currentOctave = 0;
    }

    void update (void) {
        // always update clock, will be used for MIDI clock out.
        CLOCK::update();
        // if arp is on, do arp stuff.
        if (isArpActive) {
            if (isSustainJustReleased) {
                clear_all_notes();

                // check all notes
                // for (int i = 0; i < MAX_ARP; i++) {
                //     // if its currently being sustained
                //     if (arpVoices[i].sustained) {
                //         arpVoices[i].clear();
                //         --currentNoteCount;
                //         --_write_index;
                //         NOTE_HANDLING::arpVoices_dec();
                //         if (currentNoteCount < 0) currentNoteCount = 0; // maybe add a line here to reset range and play index? 
                //         if (_write_index <= 0) _write_index = currentNoteCount;
                //         _notes_changed = true;
                //     }
                // }
                isSustainJustReleased = false;
            }
            organise_notes();
            if (CLOCK::get_changed()) {

                switch (note_state) {

                    case NOTE_ACTIVE:
                        NOTE_HANDLING::voice_off(currentVoiceIndex, currentPlayNote, 0);
                        currentVoiceIndex++;
                        if (currentVoiceIndex >= POLYPHONY) currentVoiceIndex = 0;

                        arpeggiate(_direction);
                        note_state = IDLE;
                        if (isRestEnabled) break; // comment to remove gap between notes (goes stright into next switch function instead of waiting)
                        
                    case IDLE:
                        if (currentPlayIndex >= currentNoteCount) currentPlayIndex = 0;
                        
                        if (arpVoices[currentPlayIndex].isActive()) {
                            currentPlayNote = ((arpVoices[currentPlayIndex].play())+(currentOctave*12));

                            NOTE_HANDLING::voice_on(currentVoiceIndex, currentPlayNote, 127);

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
            if (arpVoices[i].note == note) {
                _notes_changed = true;
                return;
            }
        }

        // played order - don't rearrange if you want played order.
        arpVoices[_write_index].note = note;
        ++currentNoteCount;
        ++_write_index;
        NOTE_HANDLING::voices_inc();
        if (currentNoteCount >= MAX_ARP) {
            currentNoteCount = MAX_ARP;
        }
        if (_write_index >= MAX_ARP) {
            _write_index = 0;
        }
        _notes_changed = true;
    }
    void remove_note (uint8_t note) {
        if (isHoldEnabled) {
            for (int i = 0; i <= currentNoteCount; i++) {
                if (arpVoices[i].note == note) {
                    arpVoices[i].sustain();
                    break; 
                }
            }
            _notes_changed = true;
            return;
        }
        if (currentNoteCount == 0) {
            return;
        }
        for (int i = 0; i <= currentNoteCount; i++) {
            if (arpVoices[i].note == note) {
                // Shift all the notes after the removed note back by one
                for (int j = i; j < currentNoteCount; j++) {
                    arpVoices[j].clear();
                    arpVoices[j] = arpVoices[j + 1];
                    arpVoices[j + 1].clear();
                }
                // Decrement currentNoteCount
                --currentNoteCount;
                --_write_index;
                NOTE_HANDLING::voices_dec();
                if (currentNoteCount < 0) currentNoteCount = 0; // maybe add a line here to reset range and play index? 
                if (_write_index <= 0) _write_index = currentNoteCount;
                _notes_changed = true;
                //don't return here in case the note is in the list more than once
            }
        }
    }
    // void clear_note (uint8_t slot) {
    //     arpVoices[slot].clear();
    //     --currentNoteCount;
    //     --_write_index;
    //     NOTE_HANDLING::arpVoices_dec();
    //     if (currentNoteCount < 0) currentNoteCount = 0; // maybe add a line here to reset range and play index? 
    //     if (_write_index <= 0) _write_index = currentNoteCount;
    //     _notes_changed = true;
    // }
    void organise_notes () {
        if (_notes_changed) {
            
            uint8_t length      = currentNoteCount;   // how many entries in the array
            ArpData swap;

            for (int i = 0; i < length; i++) {     
                for (int j = i+1; j < length; j++) {
                    if(arpVoices[i].note > arpVoices[j].note) {    
                        swap = arpVoices[i];    
                        arpVoices[i] = arpVoices[j];    
                        arpVoices[j] = swap;    
                    }     
                }
            }
            _notes_changed = false;
        }
    }

    void clear_all_notes () {
        for (int i = 0; i < MAX_ARP; i++) {
            arpVoices[i].clear();
        }
        _write_index = 0;
        currentNoteCount = 0;
        currentOctave = 0;
        currentPlayIndex = 0;
        NOTE_HANDLING::voices_clr();
    }

    void clear_held_notes () {
        for (int i = 0; i < MAX_ARP; i++) {
            if (arpVoices[i].sustained) {
                arpVoices[i].clear();
                NOTE_HANDLING::voices_dec();
                currentNoteCount--;
                // _write_index--;
            }
        }
    }
    
    // functions for start/stop of arp
    void pass_notes () {
        NOTE_HANDLING::voices_set(currentNoteCount);
        for (int i = 0; i < MAX_ARP; i++) {
            NOTE_HANDLING::voice_on(i, arpVoices[i].note, 127);
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
        if (isHoldEnabled != temp) {
            isHoldEnabled = temp;
            if (!isHoldEnabled) {
                // Only clears the notes if hold has been disengaged - lets you play notes then engage whatevers being held
                // isSustainJustReleased = true;
                clear_held_notes();
            }
        }
    }
    void set_sustain (bool sus) {
        if (isHoldEnabled != sus) {
            isHoldEnabled = sus;
            if (!isHoldEnabled) {
                // if sustain has been released...
                // raise flag to clear notes out?
                isSustainJustReleased = true;
            } 
        }
    }

    // working functions that dont need changing
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
        // if (currentOctave > _range) currentOctave = _range; // if you change range while playing it will pull it back immediately, instead of waiting till next range check
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
    void set_rest (uint16_t rest) {
        bool temp = (bool)(rest >> 9);
        if (isRestEnabled != temp) isRestEnabled = temp;
    }
    void set_bpm (uint16_t bpm) {
        CLOCK::set_bpm(map(bpm, KNOB_MIN, KNOB_MAX, 30, 350));
    }
}