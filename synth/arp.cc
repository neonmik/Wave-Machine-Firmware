#include "arp.h"

namespace ARP {

    ArpData arpVoices[MAX_ARP];
    ArpData inputBuffer[MAX_ARP];


    void printInputBuffer (void) {
        printf("inputBuffer         :   ");
            for (int i = 0; i < POLYPHONY; i++) {
                printf("    %d:   %02d  |", i, inputBuffer[i].note);
            }
            printf("\n");
    }
    void printArpBuffer (void) {
        printf("arpBuffer           :   ");
            for (int i = 0; i < POLYPHONY; i++) {
                printf("    %d:   %02d  |", i, arpVoices[i].note);
            }
            printf("\n");
    }


    void Init () {
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
            case ArpDirection::UP:
                ++currentPlayIndex;
                if (currentPlayIndex >= currentNoteCount) {
                    currentPlayIndex = 0;
                    updateOctave(true);
                }
                break;
            case ArpDirection::DOWN:
                currentPlayIndex--;
                if (currentNoteCount > 1) {
                    if (currentPlayIndex <= -1) {
                        currentPlayIndex = currentNoteCount - 1;
                        updateOctave(true);
                    }
                } else {
                    currentPlayIndex = 0;
                    updateOctave(true);
                }
                break;
            case ArpDirection::UP_DOWN:
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
                        updateOctave(true);
                    }
                }
                break;
            case ArpDirection::DOWN_UP:
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
                        updateOctave(true);
                    }
                }
                break;
        }
    }
    void updateOctave (bool rising) {
        switch (rising) {
            case true:
                ++currentOctave;
                if (currentOctave > _range) currentOctave = 0;
                break;
            case false:
                --currentOctave;
                if (currentOctave < _range) currentOctave = _range;
                break;
        }
        // add variable for direction for new arp modes (more similar in handling to JUNO)
    }

    void Update (void) {
        // always update clock, will be used for MIDI clock out.
        CLOCK::Update();
        // if arp is on, do arp stuff.
        if (isArpActive) {
            if (isSustainJustReleased) {
                clear_all_notes();
                isSustainJustReleased = false;
            }
            transferNotes();
            if (CLOCK::get_changed()) {

                switch (currentNoteState) {

                    case NoteState::ACTIVE:
                        NOTE_HANDLING::voice_off(currentVoiceIndex, currentPlayNote, 0);
                        currentVoiceIndex++;
                        if (currentVoiceIndex >= POLYPHONY) currentVoiceIndex = 0;

                        arpeggiate(arpDirection);
                        currentNoteState = NoteState::IDLE;
                        if (isRestEnabled) break; // comment to remove gap between notes (goes stright into next switch function instead of waiting)
                        
                    case NoteState::IDLE:
                        if (currentPlayIndex >= currentNoteCount) currentPlayIndex = 0;

                        if (arpVoices[currentPlayIndex].isActive()) {
                            currentPlayNote = ((arpVoices[currentPlayIndex].play())+(currentOctave*12));

                            NOTE_HANDLING::voice_on(currentVoiceIndex, currentPlayNote, 127);

                            currentNoteState = NoteState::ACTIVE;
                        }
                        break;
                }
                CLOCK::set_changed(false);
            }
        }
    }

    
    // Add notes to the arpeggiator input buffer
    void addNote (uint8_t note) {
        if (note == 0) {
            return;
        }
        if (!inputBufferFull) {
            // If the input buffer isn't full
            for (int i = 0; i < POLYPHONY; ++i) {
                // Check for the first empty slot
                if (inputBuffer[i].note == 0) {
                    // Set the empty slot as the new note
                    inputBuffer[i].add(note);
                    printInputBuffer();

                    // Increment the note count
                    ++inputNoteCount;

                    // Cap the note count and set buffer full flag
                    if (inputNoteCount >= POLYPHONY) {
                        inputNoteCount = POLYPHONY;
                        inputBufferFull = true;
                    }

                    // Mark that the input buffer notes have been changed
                    inputNotesUpdated = true;

                    // trigger Filter envelope
                    NOTE_HANDLING::voices_inc();
                    break;
                }
            }
        } else {
            // Check if the note is already in use
            for (int i = 0; i < POLYPHONY; ++i) {
                if (inputBuffer[i].note == note) {
                    // resets the note
                    inputBuffer[i].add(note);
                    printInputBuffer();
                    inputNotesUpdated = true;
                    return;
                }
            }

            // Write the note to the last available slot
            inputBuffer[inputWriteIndex].add(note);
            printInputBuffer();

            // Increment and wrap the write index
            inputWriteIndex = (inputWriteIndex + 1) % POLYPHONY;

            // Mark that the input buffer notes have been changed
            inputNotesUpdated = true;

            // trigger Filter envelope
            NOTE_HANDLING::voices_inc();
        }
    }

    // Remove notes to the arpeggiator input buffer
    void removeNote(uint8_t note) {
        // If Hold/Sustain is on, we don't remove notes
        if (isHoldEnabled) {
            // Check if this note is in the buffer
            for (int i = 0; i < POLYPHONY; ++i) {
                // If the note is active
                if (inputBuffer[i].note == note) {
                    // Set it to sustain
                    inputBuffer[i].sustain();
                    inputNotesUpdated = true;

                    // dont break as there may be multiple notes?
                }
            }
            // Exit function as there's nothing more to do
            return;
        } else {
            // If Hold/Sustain is not on, we remove notes
            // If the input buffer isn't full
            volatile uint8_t bufferSize;

            // If inputBuffer is full, use all the notes; if not, use however many there are
            if (inputBufferFull) {
                bufferSize = POLYPHONY;
            } else {
                bufferSize = inputNoteCount;
            }
            
            // Check all the active notes
            for (int i = 0; i < bufferSize; ++i) {
                // If the note is here
                if (inputBuffer[i].note == note) {
                    // Shift elements to remove the note
                    for (int swap = i; swap < bufferSize; ++swap) {
                        inputBuffer[swap] = inputBuffer[swap + 1];
                    }
                    // Remove the last inputBuffer
                    inputBuffer[bufferSize - 1].remove();
                    printInputBuffer();

                    // Decrement the note count
                    --inputNoteCount;

                    if (inputBufferFull) inputBufferFull = false;
                    else if (inputNoteCount < 0) { 
                        inputNoteCount = 0;
                    }

                    // Update inputWriteIndex if necessary
                    if (inputWriteIndex == i) {
                        inputWriteIndex = bufferSize - 1;
                    } else if (inputWriteIndex > i) {
                        --inputWriteIndex;
                    }

                    inputNotesUpdated = true;

                    // Release Filter envelope
                    NOTE_HANDLING::voices_dec();
                    // dont break incase it's not been removed
                }
            }
            // If it's been removed, return
            return;
        }
    }
    // Transfer notes from the arpeggiator input buffer to the playing arpeggiator
    void transferNotes () {
        if (inputNotesUpdated) {
            
            for (int copy = 0; copy < POLYPHONY; copy++) {
                arpVoices[copy] = inputBuffer[copy];
            }

            uint8_t length      = inputNoteCount;   // how many entries in the array
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
            currentNoteCount = inputNoteCount;

            inputNotesUpdated = false;

            printArpBuffer();
        }
    }

    
    void clearSustainedNotes (void) {
        uint8_t inputNotesRemoved;

        for (int i = 0; i < POLYPHONY; ++i) {
            // If the note is active
            if (inputBuffer[i].isSustained()) {
                // Set it to sustain
                inputBuffer[i].remove();
                // NOTE_HANDLING::voices_dec();

                ++inputNotesRemoved;

                inputNotesUpdated = true;
                // dont break as there may be multiple notes?

                // Exit loop as it's been found and marked
                // break;
            }
        }

        inputNoteCount -= inputNotesRemoved;
        
        transferNotes();
    }


    void clearAllNotes (void) {

    }

    void clear_all_notes () {
        for (int i = 0; i < MAX_ARP; i++) {
            arpVoices[i].remove();
        }
        _write_index = 0;
        currentNoteCount = 0;
        currentOctave = 0;
        currentPlayIndex = 0;
        NOTE_HANDLING::voices_clr();
    }

    void clear_held_notes () {
        for (int i = 0; i < MAX_ARP; i++) {
            if (inputBuffer[i].sustained) {
                inputBuffer[i].remove();
                NOTE_HANDLING::voices_dec();
            }
        }
        transferNotes();
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
            addNote(NOTE_HANDLING::voices_get(i));
        }
        transferNotes();
    }
    void stop_all () {
        NOTE_HANDLING::voices_clr();
        NOTE_HANDLING::voices_stop();
    }
    
    void setHold (uint16_t hold) {
        // if (hold == _last_hold) return;
        bool temp = (bool)(hold>>9);
        if (isHoldEnabled != temp) {
            isHoldEnabled = temp;
            if (!isHoldEnabled) {
                // Only clears the notes if hold has been disengaged - lets you play notes then engage whatevers being held
                // isSustainJustReleased = true;
                // clear_held_notes();
                
                // new note clear function... test it.
                clearSustainedNotes();
            }
        }
    }
    void setSustain (bool sus) {
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
    void setDivision (uint16_t division) {
        if (division == _last_division) return;
        _last_division = division;
        CLOCK::setDivision(division);
    }
    void setRange (uint16_t range) {
        if (range == _last_range) return;
        _last_range = range;
        // bit shift to get 0-4 octaves of range for the Arp
        _range = range>>8;

        // optional unquantized range changing:
        // if (currentOctave > _range) currentOctave = _range; // if you change range while playing it will pull it back immediately, instead of waiting till next range check
    }   
    void setDirection (uint16_t direction) {
        if (direction == _last_direction) return;
        _last_direction = direction;
        // bitshift to get 0-3 for the Arp direction
        switch (direction>>8) {
            case 0:
                arpDirection = ArpDirection::UP;
                break;
            case 1:
                arpDirection =  ArpDirection::DOWN;
                break;
            case 2:
                arpDirection =  ArpDirection::UP_DOWN;
                break;
            case 3:
                arpDirection =  ArpDirection::DOWN_UP;
                break;
        }
    }
    void setRest (uint16_t rest) {
        bool temp = (bool)(rest >> 9);
        if (isRestEnabled != temp) isRestEnabled = temp;
    }
    void setBpm (uint16_t bpm) {
        CLOCK::setBpm(map(bpm, KNOB_MIN, KNOB_MAX, 30, 350));
    }
}