#include "arp.h"

#include "../midi.h"

namespace ARP {

    ArpData arpVoices[MAX_ARP];
    ArpData inputBuffer[MAX_ARP];


    void printNoteBuffer (ArpData *input) {
        printf("Arp Note Buffer :");
            for (int i = 0; i < MAX_ARP; i++) {
                printf("  %d:  %02d  |", i, input[i].note);
            }
            printf("\n");
    }

    void set_state (bool state) {
        if (isArpActive == state) return; // already set
        
        isArpActive = state;
        reset();  
    }
    bool get_state (void) {
        return isArpActive;
    }



    void reset () {
        NOTE_HANDLING::voices_stop_all(); // clears the actual synth voices
        if (!isArpActive) {
            // passes notes following deactivation the arp, only if you're holding them down so they don't hold forever.
            passNotes(); // passes any notes in the arp buffer that arent held on by sustain - may need reworking for sustain
            clearAllNotes(); // clears the input buffer.
        }
        if (isArpActive) {
            grabNotes(); // grabs any active notes from the voices, this includes sustained, which I think would be best?
            // setSustain(NOTE_HANDLING::getSustain()); // to make sure sustain gets passed on?
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
                            currentPlayIndex = currentNoteCount > 1 ? currentNoteCount - 2 : 0;
                            if (octaveMode == OctaveMode::OLD) changeDirection = false;
                            else updateOctave(ArpDirection::UP);
                        }
                    } else {
                        --currentPlayIndex;
                        if (currentPlayIndex < 0) {
                            currentPlayIndex = currentNoteCount > 1 ? 1 : 0;
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
                        if (currentOctave >= octaveRange) {
                            currentOctave = octaveRange;
                            octaveDirection = ArpDirection::DOWN;
                        }
                        break;
                    case ArpDirection::DOWN:
                        --currentOctave;
                        if (currentOctave <= 0) {
                            currentOctave = 0;
                            octaveDirection = ArpDirection::UP;
                        }
                        break;
                }
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
            if (CLOCK::get_changed()) {
                CLOCK::set_changed(false);

                switch (currentNoteState) {
                    case NoteState::ACTIVE:
                        if (arpMode == ArpMode::POLY) {
                            // Niether of these seem to work? MIDI ISSUES can possibly reduce POLYPHONY to currentNoteCount
                            for (int i = 0; i < POLYPHONY; i++) {
                                // Should be able to remove this check, as it shouldn't change between playing and releasing
                                if (currentPlayOct[i]) {
                                    NOTE_HANDLING::voice_off(i, currentPlayOct[i], 0);
                                    MIDI::sendNoteOff(currentPlayOct[i], 0);
                                }   
                            }
                        } else {
                            NOTE_HANDLING::voice_off(currentVoiceIndex, currentPlayNote, 0);
                            MIDI::sendNoteOff(currentPlayNote, MIDI_DEFAULT_NOTE_OFF_VEL);
                            
                            currentVoiceIndex = (currentVoiceIndex + 1) % POLYPHONY;
                        }
                        arpeggiate(arpDirection);
                        currentNoteState = NoteState::IDLE;
                        
                        if (isRestEnabled) break; // currently handles the note gate, but will be rewritten and moved to NoteState::RELEASE
                    
                    // case NoteState::RELEASE:
                    //     break;
                        
                    case NoteState::IDLE:
                        transferNotes(); // this is here so notes only get updated before the next loop... and should always get updated?
                        if (arpMode == ArpMode::POLY) {
                            for (int i = 0; i < currentNoteCount; i++) {
                                // check this
                                if (!arpVoices[i].isActive()) break; 

                                currentPlayOct[i] = ((arpVoices[i].play())+(currentOctave*12));

                                NOTE_HANDLING::voice_on(i, currentPlayOct[i], 127);
                                MIDI::sendNoteOn(currentPlayOct[i], 127);
                            }
                        } else {
                            if (!arpVoices[currentPlayIndex].isActive()) return;

                            // This only gets called when gap is enabled, and latch is not, between holding a few notes and releasing... this is because it allows transferNotes to be called between releasing and triggering... 
                            if (currentPlayIndex > currentNoteCount) {
                                resetPlayIndex();
                            }


                            currentPlayNote = ((arpVoices[currentPlayIndex].play())+(currentOctave*12));

                            NOTE_HANDLING::voice_on(currentVoiceIndex, currentPlayNote, 127);
                            MIDI::sendNoteOn(currentPlayNote, 127);

                        }
                        currentNoteState = NoteState::ACTIVE;
                }
            }
        }
    }

    
    // Add a note to the arpeggiator input buffer
    void addNote (uint8_t note) {
        // Check if the note is valid
        if (note == 0) return; // Invalid note, nothing to do

        if (latchEnabled) {
            // Handle latch behavior
            if (latchRefresh) { 
                clearAllNotes();
                latchRefresh = false;
                refreshPlayIndex = true;
            }   

            ++latchCount;
            
            // printf("++latchCount: %d\n", latchCount);

            if (latchCount >= MAX_ARP) {
            latchCount = MAX_ARP; // Cap latchCount at MAX_ARP
        }
        }

        // Check if the note already exists in the inputBuffer
        for (int i = 0; i < MAX_ARP; ++i) {
            if (inputBuffer[i].note == note) {
                inputBuffer[i].add(note);
                inputNotesUpdated = true;
                return; // Note found, no need to proceed further
            }
        }
        if (!inputBufferFull) {
            // Find the first empty slot in the inputBuffer
            for (int i = 0; i < MAX_ARP; ++i) {
                if (inputBuffer[i].note == 0) {
                    inputBuffer[i].add(note);
                    inputNotesUpdated = true;
                    // NOTE_HANDLING::voices_inc();
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
            inputBuffer[inputWriteIndex].add(note);
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

                    // Update inputWriteIndex if necessary
                    if (inputWriteIndex == i) {
                        inputWriteIndex = bufferSize - 1;
                    } else if (inputWriteIndex > i) {
                        --inputWriteIndex;
                    }

                    inputNotesUpdated = true;

                    // Continue the loop in case there are multiple entries for the same note
                }
            } 
        }
    }

    // Transfer notes from the arpeggiator input buffer to the playing arpeggiator
    void transferNotes () {
        if (inputNotesUpdated) {
            
            // printNoteBuffer(inputBuffer);

            memcpy(arpVoices, inputBuffer, sizeof(inputBuffer));

            uint8_t length      = inputNoteCount;   // how many entries in the array

            // if (!playedOrder) { // would allow the notes to not be organised for a mode, this should still mean all the zeros are at the top...
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

            // }

            if (refreshPlayIndex) {
                if (currentPlayIndex > 0) resetPlayIndex();
                // resetOctave();
                refreshPlayIndex = false;
            }
            if (currentPlayIndex > 0 && currentPlayIndex >= inputNoteCount) {
                resetPlayIndex();
            }

            currentNoteCount = inputNoteCount;

            inputNotesUpdated = false;
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
        if (isSustainEnabled) {
            isSustainEnabled = false; // temproarily disable the hold buffer while we clear the notes
            // actually remove notes marked for removal
            for (int i = 0; i < notesForRemoval; ++i) {
                removeNote(notesToRemove[i]);
                // probably not needed if arp is playing?
                // MIDI::sendNoteOff(notesToRemove[i], MIDI_DEFAULT_NOTE_OFF_VEL); 
            }
            isSustainEnabled = true;
        } else {
            for (int i = 0; i < notesForRemoval; ++i) {
                removeNote(notesToRemove[i]);
            }
        }
        
        // transferNotes(); // should get called automatically by clock updating


        currentOctave = 0; // may need to be done on transfer notes...
        // currentPlayIndex = 0; gets done on transfer notes...

        latchCount = 0; // should be handled on transfer notes...
    }

    void passNotes (void) {
        if (!isSustainEnabled) {
            // sustain not active - send all actual notes
            for (int i = 0; i < MAX_ARP; i++) {
                // copy all the notes form arp to normal voices
                uint8_t note = arpVoices[i].note;
                if (note != 0) {
                    NOTE_HANDLING::voice_on(i, note, 127);
                    MIDI::sendNoteOn(note, 127);
                }
            }
        } else {
            // sustain is active - only pass held notes, we want the sustained notes to ring out
            for (int i = 0; i < MAX_ARP; i++) {
                // if it's sustained, don't pass it for now.
                if (arpVoices[i].isSustained()) break;

                uint8_t note = arpVoices[i].note;
                // check there is a note number, if not, skip it
                if (note == 0)  break;
                
                // pass the note out
                NOTE_HANDLING::voice_on(i, note, 127);
                MIDI::sendNoteOn(note, 127);

                // // mark the note as sustained if it is 
                // if (arpVoices[i].isSustained()) NOTE_HANDLING::note_off(note, MIDI_DEFAULT_NOTE_OFF_VEL); 
            }
        }
    }

    void grabNotes (void) {
        uint8_t sustainNotes = 0;

        for (int i = 0; i < POLYPHONY; i++) {
            // this loops through all the active voices, adding it to the inputBuffer.
            uint8_t note = NOTE_HANDLING::voices_get(i);
            bool sustained = NOTE_HANDLING::voices_check(i);
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
        // transferNotes(); // dont think this needs to be here, Arp should always update notes inside loop
    }
    void stopAllVoices () {
        // NOTE_HANDLING::voices_clr();
        NOTE_HANDLING::voices_stop();
    }

    
    void toggleHold (void) {
        isSustainEnabled = !isSustainEnabled; // toggle

        updateSustain();
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
        // if (currentOctave > octaveRange) currentOctave = octaveRange; // if you change range while playing it will pull it back immediately, instead of waiting till next range check
    }   
    void setDirection (uint16_t input) {
        if (direction == (input>>8)) return;
        direction = (input>>8);
        // bitshift to get 0-3 for the Arp direction
        switch (direction) {
            case 0:
                arpDirection = ArpDirection::UP;
                octaveDirection = ArpDirection::UP;
                break;
            case 1:
                arpDirection =  ArpDirection::DOWN;
                octaveDirection = ArpDirection::DOWN;
                break;
            case 2:
                arpDirection =  ArpDirection::UP_DOWN;
                octaveDirection = ArpDirection::UP;
                break;
            case 3:
                arpDirection =  ArpDirection::DOWN_UP;
                octaveDirection = ArpDirection::DOWN;
                break;
        }
    }
    void setGate (uint16_t input) {
        bool temp = (bool)(input >> 9);
        if (isRestEnabled != temp) isRestEnabled = temp;
    }
    void setBpm (uint16_t input) {
        CLOCK::setBpm(map(input, KNOB_MIN, KNOB_MAX, 30, 350));
    }
}