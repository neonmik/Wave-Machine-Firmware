#include "arp.h"

#include "../midi.h"

namespace ARP {

    ArpData arpVoices[MAX_ARP];
    ArpData inputBuffer[MAX_ARP];


    void printNoteBuffer (ArpData *input) {
        printf("inputBuffer :");
            for (int i = 0; i < MAX_ARP; i++) {
                printf("  %d:  %02d  |", i, input[i].note);
            }
            printf("\n");
    }

    void Init () {
        // nothing happens here, but it might?
    }

    void set_state (bool state) {
        if (state != isArpActive) {
            isArpActive = state;
            reset();  
        }
    }
    bool get_state (void) {
        return isArpActive;
    }

    void reset () {
        if (!isArpActive) {
            // passes notes following deactivation the arp, only if you're holding them down so they don't hold forever.
            NOTE_HANDLING::voices_stop_all(); // clears the actual synth voices
            passNotes(); // passes any notes in the arp buffer that arent held on by sustain - may need reworking for sustain
            clearAllNotes(); // clears the input buffer.
        }
        if (isArpActive) {
            NOTE_HANDLING::voices_stop_all(); // stops the actual synth voices to prevent notes hanging
            grabNotes(); // grabs any active notes from the voices, this includes sustained, which I think would be best?
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
                clearSustainedNotes();
                isSustainJustReleased = false;
            }
            transferNotes();
            if (CLOCK::get_changed()) {

                switch (currentNoteState) {

                    case NoteState::ACTIVE:
                        NOTE_HANDLING::voice_off(currentVoiceIndex, currentPlayNote, 0);
                        MIDI::sendNoteOff(currentPlayNote, MIDI_DEFAULT_NOTE_OFF_VEL);
                        
                        currentVoiceIndex = (currentVoiceIndex + 1) % POLYPHONY;


                        arpeggiate(arpDirection);
                        currentNoteState = NoteState::IDLE;
                        if (isRestEnabled) break; // comment to remove gap between notes (goes stright into next switch function instead of waiting)
                        
                    case NoteState::IDLE:
                        if (currentPlayIndex >= currentNoteCount) currentPlayIndex = 0;

                        if (arpVoices[currentPlayIndex].isActive()) {
                            currentPlayNote = ((arpVoices[currentPlayIndex].play())+(currentOctave*12));

                            NOTE_HANDLING::voice_on(currentVoiceIndex, currentPlayNote, 127);
                            MIDI::sendNoteOn(currentPlayNote, 127);

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
        // check if not is already playing
        if (isHoldEnabled) {
            // if the chord refresh option is enabled
            if (chordRefresh) { 
                // refresh timeout clock
                clearAllNotes();
                chordRefreshCount = 0;
                chordRefresh = false;
                // don't return, let it roll on and add new note
            }   
            ++chordRefreshCount;
            if (chordRefreshCount == MAX_ARP) chordRefreshCount = MAX_ARP;
            printf("chordRefreshCount++ = %d\n", chordRefreshCount);

        }
        for (int i = 0; i < MAX_ARP; ++i) {
            if (inputBuffer[i].note == note) {
                // resets the note
                inputBuffer[i].add(note);
                // printNoteBuffer(inputBuffer);
                inputNotesUpdated = true;
                return;
            }
        }
        if (!inputBufferFull) {

            // If the input buffer isn't full
            for (int i = 0; i < MAX_ARP; ++i) {
                // Check for the first empty slot
                if (inputBuffer[i].note == 0) {
                    // Set the empty slot as the new note
                    inputBuffer[i].add(note);


                    // Mark that the input buffer notes have been changed
                    inputNotesUpdated = true;

                    // trigger Filter envelope
                    NOTE_HANDLING::voices_inc();

                    // Increment the note count
                    ++inputNoteCount;
                    if (inputNoteCount >= MAX_ARP) {
                        inputNoteCount = MAX_ARP;
                        inputBufferFull = true;
                    }
                    break;
                }
            }
            //return here so it doesnt roll over
            // return;
        } else {
            // Write the note to the last available slot
            inputBuffer[inputWriteIndex].add(note);
            // printNoteBuffer(inputBuffer);

            // Increment and wrap the write index
            inputWriteIndex = (inputWriteIndex + 1) % MAX_ARP;

            // Mark that the input buffer notes have been changed
            inputNotesUpdated = true;

            // trigger Filter envelope
            NOTE_HANDLING::voices_inc();
        }
    }
    // Remove notes to the arpeggiator input buffer
    void removeNote(uint8_t note) {
        // If Hold/Sustain is not on, we remove notes
        // If the input buffer isn't full
        volatile uint8_t bufferSize;

        // If inputBuffer is full, use all the notes; if not, use however many there are
        if (inputBufferFull) {
            bufferSize = MAX_ARP;
        } else {
            bufferSize = inputNoteCount;
        }
        if (isHoldEnabled) { 
            if (chordRefreshLatching) {   
                // new chord type of arp latching - allows you to actually play it without a pedal
                chordRefreshCount--;
                printf("chordRefreshCount-- = %d\n", chordRefreshCount);
                if (chordRefreshCount <= 0) {
                    chordRefreshCount = 0;
                    chordRefresh = true;
                }

                // still carry on to mark as sustained
            }
        }
        // Check all the active notes
        for (int i = 0; i < bufferSize; ++i) {
            // If the note is here
            if (inputBuffer[i].note == note) {
                if (isHoldEnabled) {
                    // Set it to sustain
                    inputBuffer[i].sustain();
                    inputNotesUpdated = true;

                    break;
                } else {
                    // Shift elements to remove the note
                    for (int swap = i; swap < bufferSize; ++swap) {
                        inputBuffer[swap] = inputBuffer[swap + 1];
                    }
                    // Remove the last inputBuffer
                    inputBuffer[bufferSize - 1].remove();
                    // printNoteBuffer(inputBuffer);

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
        }
    }
    // Transfer notes from the arpeggiator input buffer to the playing arpeggiator
    void transferNotes () {
        if (inputNotesUpdated) {
            
            printNoteBuffer(inputBuffer);

            memcpy(arpVoices, inputBuffer, sizeof(inputBuffer));

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

            // printNoteBuffer(arpVoices);
        }
    }
    // Clear sustained notes from the input buffer and then update the the arpeggiator buffer
    void clearSustainedNotes (void) {
        uint8_t notesToRemove[MAX_ARP];
        uint8_t notesForRemoval;
        uint8_t bufferSize; 
        
        if (inputBufferFull) {
            bufferSize = MAX_ARP;
        } else {
            bufferSize = inputNoteCount;
        }
    
        // check every active note
        for (int i = 0; i < bufferSize; ++i) {
            // If the note is sustained
            if (inputBuffer[i].isSustained()) {
                // clear sustain
                inputBuffer[i].sustained = false;
                // add to list for removal afterwards - messed the order up if done here
                notesToRemove[notesForRemoval] = inputBuffer[i].note;
                // increase amount of notes to be removed
                ++notesForRemoval;
            }
        }
        // actually remove notes marked for removal
        for (int i = 0; i < notesForRemoval; ++i) {
            removeNote(notesToRemove[i]);
            MIDI::sendNoteOff(notesToRemove[i], MIDI_DEFAULT_NOTE_OFF_VEL);
        }
        
        transferNotes();
    }
    
    // Clear all notes from the input buffer and then update the arpeggiator buffer
    void clearAllNotes (void) {
        uint8_t notesToRemove[MAX_ARP];
        uint8_t notesForRemoval;
    
        // check every active note
        for (int i = 0; i < MAX_ARP; ++i) {
            // If the note has a note
            if (inputBuffer[i].note != 0) {
                // maybe add some sustain protection here? this should only be called on starting/stopping the arp

                // add to list for removal afterwards - messed the order up if done here
                notesToRemove[notesForRemoval] = inputBuffer[i].note;
                // increase amount of notes to be removed
                ++notesForRemoval;
            }
        }
        if (isHoldEnabled) {
            isHoldEnabled = false; // temproarily disable the hold buffer while we clear the notes
            // actually remove notes marked for removal
            for (int i = 0; i < notesForRemoval; ++i) {
                removeNote(notesToRemove[i]);
                // probably not needed if arp is playing?
                MIDI::sendNoteOff(notesToRemove[i], MIDI_DEFAULT_NOTE_OFF_VEL);
            }
            isHoldEnabled = true;
        } else {
            for (int i = 0; i < notesForRemoval; ++i) {
                removeNote(notesToRemove[i]);
            }
        }
        
        transferNotes();

        currentOctave = 0;
        currentPlayIndex = 0;


        NOTE_HANDLING::voices_clr();
    }

    void passNotes (void) {
        // for passing active notes through to voices when the arp is turned off...
        uint8_t activeVoices;

        if (!isHoldEnabled) {
            // sustain not active - release all notes
            for (int i = 0; i < MAX_ARP; i++) {
                // copy all the notes form arp to normal voices
                uint8_t note = arpVoices[i].note;
                if (note != 0) {
                    NOTE_HANDLING::voice_on(activeVoices, note, 127);
                    MIDI::sendNoteOn(note, 127);
                    activeVoices++;
                }
            }
            // just a little sanity check
            if (activeVoices != inputNoteCount) printf("Passing Notes count mismatch!");
            // update the filter envelope
            NOTE_HANDLING::voices_set(activeVoices);
        } else {
            // sustain is active - only release sustained notes, we want the notes that are held down to keep playing out
            for (int i = 0; i < MAX_ARP; i++) {
                uint8_t note = inputBuffer[i].note;
                // check there is a note numbe, if not, skip it
                if (note == 0)  break;
                
                // if the note is not marked as being sustained, transfer it to the first available voice
                if (!inputBuffer[i].isSustained()) {
                    // note isn't marked as sustaining, therefore meaning the note is sill pressed down
                    NOTE_HANDLING::voice_on(activeVoices, note, 127);
                    MIDI::sendNoteOn(note, 127);
                    activeVoices++;
                } else {
                    // note is held with sustain, if it's playing, release it.
                    NOTE_HANDLING::voice_stop(note);
                    MIDI::sendNoteOff(note, MIDI_DEFAULT_NOTE_OFF_VEL);
                }
            }
            NOTE_HANDLING::voices_set(activeVoices);
        }
    }

    void grabNotes (void) {
        for (int i = 0; i < POLYPHONY; i++) {
            // this loops through all the active voices, adding it to the inputBuffer. this internally places it in a correct voice.
            addNote(NOTE_HANDLING::voices_get(i));
        }
        transferNotes();
    }
    void stopAllVoices () {
        NOTE_HANDLING::voices_clr();
        NOTE_HANDLING::voices_stop();
    }
    
    void setHold (uint16_t hold) {
        bool temp = (bool)(hold>>9);

        if (temp == _last_hold) return;
        _last_hold = temp;
        
        setSustain(temp);
    }
    void setSustain (bool sus) {
        if (isHoldEnabled != sus) {
            isHoldEnabled = sus;
            if (!isHoldEnabled) {
                // Only clears the notes if hold has been disengaged - lets you play notes then engage whatevers being held
                isSustainJustReleased = true;
                // copy this over for tracking the held notes
                // chordRefreshCount = inputNoteCount;
            } 
        }
    }

    // working functions that dont need changing
    void setDivision (uint16_t division) {
        // Fix this, it wavers at the top and is computationally expensive.
        uint8_t temp = map(division, 0, 1023, 0, 10);

        
        if (temp == _last_division) {
            // stops the division being updated when 
            return;
        }

        _last_division = temp;
        CLOCK::setDivision(temp);
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