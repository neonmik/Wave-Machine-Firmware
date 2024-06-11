#include "arp.h"

#include "../midi.h"

namespace ARP {

    ArpData playBuffer[MAX_ARP];
    ArpData inputBuffer[MAX_ARP];


    // Prints the note buffer
    void printNoteBuffer(ArpData *input) {
        printf("Arp Note Buffer :");
        for (int i = 0; i < MAX_ARP; i++) {
            printf("  %d:  %02d  |", i, input[i].note);
        }
        printf("\n\n");
    }

    // Sets the state of the arp
    void setState(bool state) {
        if (isArpActive == state) return; // If state is already set, do nothing
        isArpActive = state;
        reset();  
    }

    // Returns the state of the arp
    bool getState(void) {
        return isArpActive;
    }



    // Resets the arp
    void reset() {
        NOTE_HANDLING::voicesStop(); // Clears the actual synth voices
        if (!isArpActive) {
            passNotes(); // Passes any notes in the arp buffer that aren't held on by sustain
            clearAllNotes(); // Clears the input buffer
        }
        if (isArpActive) {
            grabNotes(); // Grabs any active notes from the voices, including sustained notes
        }
    }


    void arpeggiate(ArpDirection direction) {
        if (arpMode == ArpMode::POLY) {
            switch (direction) {
                case ArpDirection::UP:
                    updateOctave(ArpDirection::UP);
                    break;
                case ArpDirection::DOWN:
                    updateOctave(ArpDirection::DOWN);
                    break;
                case ArpDirection::UP_DOWN:
                    updateOctave(ArpDirection::UP_DOWN);
                    break;
                case ArpDirection::DOWN_UP:
                    updateOctave(ArpDirection::DOWN_UP);
                    break;
            }
            return;
        } else {
            switch (direction) {
                case ArpDirection::UP:
                case ArpDirection::PLAYED_ORDER:
                    ++currentPlayIndex;
                    if (currentPlayIndex >= currentNoteCount) {
                        resetPlayIndex();
                        updateOctave(ArpDirection::UP);
                    }
                    break;
                case ArpDirection::DOWN:
                    currentPlayIndex--;
                    if (currentNoteCount > 1) {
                        if (currentPlayIndex <= -1) {
                            currentPlayIndex = currentNoteCount - 1;
                            updateOctave(ArpDirection::UP);

                        }
                    } else {
                        resetPlayIndex();
                        updateOctave(ArpDirection::UP);
                    }
                    break;
                case ArpDirection::UP_DOWN:
                    if (changeDirection) {
                        ++currentPlayIndex;
                        if (currentPlayIndex >= currentNoteCount) {
                            currentPlayIndex = currentNoteCount > 1 ? currentNoteCount - 2 : 0; // if theres more than 1 not currnetly playing, set the index to the second to last note, otherwise set it to 0
                            if (octaveMode == OctaveMode::OLD) changeDirection = false;
                            else updateOctave(ArpDirection::UP);
                        }
                    } else {
                        --currentPlayIndex;
                        if (currentPlayIndex < 0) {
                            currentPlayIndex = currentNoteCount > 1 ? 1 : 0; // if theres more than one note, set the index to 1, otherwise set it to 0
                            if (octaveMode == OctaveMode::OLD) {
                                changeDirection = true;
                                updateOctave(ArpDirection::UP);
                            } else {
                                updateOctave(ArpDirection::DOWN);
                            }
                        }
                    }
                    break;
                case ArpDirection::DOWN_UP:
                    if (changeDirection) {
                        currentPlayIndex--;
                        if (currentPlayIndex < 0) {
                            currentPlayIndex = currentNoteCount > 1 ? 1 : 0;
                            if (octaveMode == OctaveMode::OLD) changeDirection = false;
                            else updateOctave(ArpDirection::DOWN);
                        }
                    } else {
                        ++currentPlayIndex;
                        if (currentPlayIndex >= currentNoteCount) {
                            currentPlayIndex = currentNoteCount > 1 ? currentNoteCount - 2 : 0;
                            if (octaveMode == OctaveMode::OLD) {
                                changeDirection = true;
                                updateOctave(ArpDirection::UP);
                            } else {
                                updateOctave(ArpDirection::UP);
                            }
                        }
                    }
                    break;
                }
        }
    }
    void updateOctave (ArpDirection direction) {
        switch (direction) {
            case ArpDirection::UP:
                ++currentOctave;
                if (currentOctave > octaveRange) {
                    currentOctave = 0;
                }
                break;
            case ArpDirection::DOWN:
                --currentOctave;
                if (currentOctave < 0) {
                    currentOctave = octaveRange;
                }
                break;
            case ArpDirection::UP_DOWN:
            case ArpDirection::DOWN_UP:
                switch (octaveDirection) {
                    case ArpDirection::UP:
                        ++currentOctave;
                        if (currentOctave > octaveRange) {
                            currentOctave = octaveRange;
                            octaveDirection = ArpDirection::DOWN;
                        }
                        break;
                    case ArpDirection::DOWN:
                        --currentOctave;
                        if (currentOctave < 0) {
                            currentOctave = 0;
                            octaveDirection = ArpDirection::UP;
                        }
                        break;
                }
                break;
        }
        // add variable for direction for new arp modes (more similar in handling to JUNO)
    }

    void playNote (void) {
        if (octaveModeChanged) { // only updates once notes have stopped playing
            if (polyMode) arpMode = ArpMode::POLY;
            else arpMode = ArpMode::MONO;
            octaveModeChanged = false;
        }

        if (arpMode == ArpMode::POLY) {
            for (int i = 0; i < currentNoteCount; i++) {
                // check this
                if (!playBuffer[i].isActive()) break; 

                currentPlayOct[i] = ((playBuffer[i].play())+(currentOctave*12));

                NOTE_HANDLING::voiceOn(i, currentPlayOct[i], 127);
                MIDI::sendNoteOn(currentPlayOct[i], 127);
            }
        } else {
            
            if (!playBuffer[currentPlayIndex].isActive()) return;

            // This only gets called when gap is enabled, and latch is not, between holding a few notes and releasing... this is because it allows transferNotes to be called between releasing and triggering... 
            if (currentPlayIndex > currentNoteCount) {
                resetPlayIndex();
            }


            currentPlayNote = ((playBuffer[currentPlayIndex].play())+(currentOctave*12));

            NOTE_HANDLING::voiceOn(currentVoiceIndex, currentPlayNote, 127);
            MIDI::sendNoteOn(currentPlayNote, 127);

        }
        currentNoteState = NoteState::ACTIVE;
    }
    void releaseNote(void) {
        if (arpMode == ArpMode::POLY) {
            for (int i = 0; i < POLYPHONY; i++) {
                // Should be able to remove this check, as it shouldn't change between playing and releasing
                if (currentPlayOct[i]) {
                    NOTE_HANDLING::voiceOff(i, currentPlayOct[i], 0);
                    MIDI::sendNoteOff(currentPlayOct[i], 0);
                }   
            }
        } else {
            NOTE_HANDLING::voiceOff(currentVoiceIndex, currentPlayNote, 0);
            MIDI::sendNoteOff(currentPlayNote, MIDI_DEFAULT_NOTE_OFF_VEL);

            currentPlayNote = 0; // clear playing note once it's release
            
            currentVoiceIndex = (currentVoiceIndex + 1) % POLYPHONY;
        }
        arpeggiate(arpDirection);
        currentNoteState = NoteState::IDLE;
    }

    void update (void) {
        // always update clock, will be used for MIDI clock out.
        CLOCK::update();
        // if arp is on, do arp stuff.
        if (isArpActive) {
            if (isSustainJustReleased) {
                clearSustainedNotes();
                isSustainJustReleased = false;
            }
            uint32_t currentNoteTick = CLOCK::getClockTick();
             if ((currentNoteTick >= gate && currentNoteState == NoteState::ACTIVE)) { // && !CLOCK::getClockChanged()) {
                currentNoteState = NoteState::RELEASE;
                releaseNote();
            }
             if (CLOCK::getClockChanged()) {
                CLOCK::resetClockChanged();
                switch(currentNoteState) {
                    case NoteState::ACTIVE:
                        currentNoteState = NoteState::RELEASE;
                    case NoteState::RELEASE:
                        releaseNote();
                    case NoteState::IDLE:
                        transferNotes(); // this is here so notes only get updated before the next loop... and should always get updated?
                        playNote();
                }
            }
        }
    }

    void checkLatch (void) {
        if (!latchEnabled) return;
    
        // Handle latch behavior
        if (latchRefresh) { 
            clearAllNotes();
            latchRefresh = false;
            refreshPlayIndex = true;
        }   

        ++latchCount;

        if (latchCount >= MAX_ARP) {
            latchCount = MAX_ARP; // Cap latchCount at MAX_ARP
        }
    }
    
    // Add a note to the arpeggiator input buffer
    void addNote (uint8_t note, uint8_t velocity) {
        // Check if the note is valid
        if (note == 0) return; // Invalid note, nothing to do

        checkLatch();

        // Check if the note already exists in the inputBuffer - this may just want removing to allow for multiple notes being played into a sequence
        
        // for (int i = 0; i < MAX_ARP; ++i) {
        //     if (inputBuffer[i].note == note) {
        //         //retrigger note
        //         inputBuffer[i].add(note, velocity);
        //         inputNotesUpdated = true;
        //         return; // Note found, no need to proceed further
        //     }
        // }
        if (!inputBufferFull) {
            // Find the first empty slot in the inputBuffer
            for (int i = 0; i < MAX_ARP; ++i) {
                if (inputBuffer[i].note == 0) {
                    inputBuffer[i].add(note, velocity);
                    inputNotesUpdated = true;
                    ++inputNoteCount;

                    if (inputNoteCount >= MAX_ARP) {
                        inputNoteCount = MAX_ARP;
                        inputBufferFull = true;
                    }

                    return; // Note added to an empty slot
                }
            }
        } else {
            // Write the note to the last available slot and wrap the index
            inputBuffer[inputWriteIndex].add(note, velocity);
            inputWriteIndex = (inputWriteIndex + 1) % MAX_ARP;
            inputNotesUpdated = true;
        }
    }
    // Remove a note from the arpeggiator input buffer
    void removeNote(uint8_t note) {
        volatile uint8_t bufferSize;

        // Determine the buffer size based on whether it's full or not
        if (inputBufferFull) {
            bufferSize = MAX_ARP; // Use all available slots
        } else {
            bufferSize = inputNoteCount; // Use the number of active notes
        }
        
        if (latchEnabled) {
            // Handle latch behavior
            if (latchCount > 0) {
                latchCount--;

                // printf("--latchCount: %d\n", latchCount);

                if (latchCount == 0) {
                    latchRefresh = true;
                }
            }
        }

        // Iterate through the active notes in the buffer
        for (int i = 0; i < bufferSize; ++i) {
            // Check if the note is in the buffer
            if (inputBuffer[i].note == note) {
                if (isSustainEnabled) {
                    // Set the note to sustain
                    inputBuffer[i].sustain();
                    inputNotesUpdated = true;

                    break; // Exit the loop as we've handled the note
                } else {
                    // Shift elements to remove the note
                    for (int swap = i; swap < bufferSize - 1; ++swap) {
                        inputBuffer[swap] = inputBuffer[swap + 1];
                    }
                    // Remove the last inputBuffer
                    inputBuffer[bufferSize - 1].remove();
                    
                    // Decrement the note count
                    --inputNoteCount;

                    if (inputBufferFull) {
                        inputBufferFull = false;
                    } else if (inputNoteCount < 0) { 
                        inputNoteCount = 0;
                    }

                    // update inputWriteIndex if necessary
                    if (inputWriteIndex == i) {
                        inputWriteIndex = bufferSize - 1;
                    } else if (inputWriteIndex > i) {
                        --inputWriteIndex;
                    }

                    inputNotesUpdated = true;

                    // Continue the loop in case there are multiple entries for the same note - this will definetly be the case if I allow it.
                }
            } 
        }
    }


    // Transfer notes from the arpeggiator input buffer to the playing arpeggiator
    void transferNotes () {
        if (inputNotesUpdated) {
            
            // printNoteBuffer(inputBuffer);

            memcpy(playBuffer, inputBuffer, sizeof(inputBuffer));

            uint8_t length      = inputNoteCount;   // how many entries in the array

            if (!playedOrder) { // would allow the notes to not be organised for a mode, this should still mean all the zeros are at the top...
                ArpData swap;

                for (int i = 0; i < length; i++) {     
                    for (int j = i+1; j < length; j++) {
                        if(playBuffer[i].note > playBuffer[j].note) {
                            swap = playBuffer[i];    
                            playBuffer[i] = playBuffer[j];
                            playBuffer[j] = swap;    
                        }     
                    }
                }

            }

            if (refreshPlayIndex) {
                if (currentPlayIndex > 0) resetPlayIndex();
                resetOctave();
                refreshPlayIndex = false;
            }
            if (currentPlayIndex > 0 && currentPlayIndex >= inputNoteCount) {
                resetPlayIndex();
            }

            currentNoteCount = inputNoteCount;

            inputNotesUpdated = false;
        }
    }
    
    
    void clearNote (uint8_t note) {
        removeNote(note);
        MIDI::sendNoteOff(note, MIDI_DEFAULT_NOTE_OFF_VEL);
    }
    void removeNotes(const uint8_t* notesToRemove, uint8_t count) {
        for (int i = 0; i < count; ++i) {
            clearNote(notesToRemove[i]);
        }
    }
    
    // Clear sustained notes from the input buffer and then update the the arpeggiator buffer
    void clearSustainedNotes (void) {
        uint8_t notesToRemove[MAX_ARP];
        uint8_t notesForRemoval = 0;
        uint8_t bufferSize = inputBufferFull ? MAX_ARP : inputNoteCount;
    
        // Check notes for sustain and remove if sustained
        for (int i = 0; i < bufferSize; ++i) {
            if (inputBuffer[i].isSustained()) {
                inputBuffer[i].sustained = false;
                notesToRemove[notesForRemoval++] = inputBuffer[i].note;
            }
        }

        // Clear only the previously sustained notes
        removeNotes(notesToRemove, notesForRemoval);
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
        if (isSustainEnabled) {
            isSustainEnabled = false; // temproarily disable the hold buffer while we clear the notes
            // actually remove notes marked for removal
            for (int i = 0; i < notesForRemoval; ++i) {
                removeNote(notesToRemove[i]);
            }
            isSustainEnabled = true;
        } else {
            for (int i = 0; i < notesForRemoval; ++i) {
                removeNote(notesToRemove[i]);
            }
        }

        currentOctave = 0; // may need to be done on transfer notes...
        // currentPlayIndex = 0; gets done on transfer notes...

        latchCount = 0; // should be handled on transfer notes...
    }

    void passNotes (void) {
        transferNotes();

        if (!isSustainEnabled) {
            // sustain not active - send all actual notes
            for (int i = 0; i < MAX_ARP; i++) {
                // copy all the notes form arp to normal voices
                uint8_t note = playBuffer[i].note;
                if (note != 0) {
                    NOTE_HANDLING::voiceOn(i, note, 127);
                    MIDI::sendNoteOn(note, 127);
                }
            }
        } else {
            // sustain is active - only pass held notes, we want the sustained notes to ring out
            for (int i = 0; i < MAX_ARP; i++) {
                // if it's sustained, don't pass it for now.
                if (playBuffer[i].isSustained()) break;

                uint8_t note = playBuffer[i].note;
                // check there is a note number, if not, skip it
                if (note == 0)  break;
                
                // pass the note out
                NOTE_HANDLING::voiceOn(i, note, 127);
                MIDI::sendNoteOn(note, 127);

                // // mark the note as sustained if it is 
                // if (playBuffer[i].isSustained()) NOTE_HANDLING::noteOff(note, MIDI_DEFAULT_NOTE_OFF_VEL); 
            }
        }

    }

    void grabNotes (void) {
        uint8_t sustainNotes = 0;

        for (int i = 0; i < POLYPHONY; i++) {
            // this loops through all the active voices, adding it to the inputBuffer.
            uint8_t note = NOTE_HANDLING::voicesActiveNote(i);
            bool sustained = NOTE_HANDLING::voicesCheckSustain(i);
            addNote(note);
            if (sustained) {
                sustainNotes++;
                for (int i = 0; i < MAX_ARP; i++) {
                    if (inputBuffer[i].note == note) {
                        inputBuffer[i].sustain(); // means if the note is already sustained, it logs that.
                    }
                }
            }
        }
        if (latchEnabled) {
            latchCount -= sustainNotes;

            // printf("grabNotes latchCount: %d\n", latchCount);
            
            if (latchCount <= 0) {
                latchCount = 0;
                latchRefresh = true;
            }
        }

        transferNotes(); // dont think this needs to be here, Arp should always update notes inside loop
    }
    
    void stopAllVoices () {
        NOTE_HANDLING::voicesStop();
    }

    
    void toggleHold (void) {
        isSustainEnabled = !isSustainEnabled; // toggle

        updateSustain();
    }
    bool getHold(void) {
        return isSustainEnabled;
    }
    void toggleSustain (void) {
        isSustainEnabled = !isSustainEnabled; // toggle
        
        updateSustain();
    }
    void updateSustain (void) {

        if (!isSustainEnabled) {
            // Only clears the notes if hold has been disengaged - lets you play notes then engage whatevers being held
            isSustainJustReleased = true;
        } 
    }

    // working functions that dont need changing
    void setDivision (uint16_t input) {
        // Fix this, it wavers at the top and is computationally expensive.
        uint8_t temp = map(input, 0, 1023, 0, 10);

        
        if (division == temp) return;

        division = temp;
        CLOCK::setDivision(temp);
    }
    void setRange (uint16_t input) {
        if (range == (input >> 8)) return;
        range = (input >> 8);
        // bit shift to get 0-4 octaves of range for the Arp
        octaveRange = range;

        // optional unquantized range changing:
        // if (currentOctave > octaveRange) currentOctave = octaveRange; // if you change range while playing it will pull it back to 0 immediately, instead of waiting till next range check
    }   
    void setDirection (uint16_t input) {
        if (direction == (input>>8)) return;
        direction = (input>>8);
        // bitshift to get 0-3 for the Arp direction
        switch (direction) {
            case 0:
                // Arp UP
                arpDirection = ArpDirection::UP;
                octaveDirection = ArpDirection::UP;
                break;
            case 1:
                // Arp DOWN
                arpDirection =  ArpDirection::DOWN;
                octaveDirection = ArpDirection::DOWN;
                break;
            case 2:
                // Arp UP DOWN
                arpDirection =  ArpDirection::UP_DOWN;
                octaveDirection = ArpDirection::UP;
                break;
            case 3:
                // Arp DOWN UP
                arpDirection =  ArpDirection::DOWN_UP;
                octaveDirection = ArpDirection::DOWN;
                break;
        }
    }
    void setGate (uint16_t input) {
        gate = (CLOCK::getSamplesPerDivision() * input) >> 10;

        // TODO: add dynamic minimum gate time
        if (gate < 240) gate = 240; // minimum gate time of 120 samples, any shorter and the note doesn't fire
    }
    void setBPM (uint16_t input) {
        // TODO: recode the following section so it doesnt use the map function, as that is computationally expensive
        CLOCK::setBPM(map(input, KNOB_MIN, KNOB_MAX, 30, 350));
    }
    void setOctMode (uint16_t input) {
        bool temp = (bool)(input >> 9);
        if (polyMode != temp) {
            polyMode = temp;
            octaveModeChanged = true;

            // this was moved to the running arp code to make sure notes don't hang on changing the setting
            // if (polyMode) arpMode = ArpMode::POLY;
            // else arpMode = ArpMode::MONO;
        }
    }
    void playedOrderToggle (void) {
        playedOrder = !playedOrder;

        if (playedOrder) 

        inputNotesUpdated = true; // this is here to make sure that notes are switched to played order as soon as changing the setting.

    }
    void setPlayedOrder (uint16_t input) {
        if (playedOrder == (bool)(input >> 9)) return;

        playedOrder = (bool)(input >> 9);

        inputNotesUpdated = true; // this is here to make sure that notes are switched to played order as soon as changing the setting.
    }
}