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

    void Init () {
        // nothing happens here, but it might?
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
                        currentPlayIndex = 0;
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
                        currentPlayIndex = 0;
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
                transferNotes(); // moving in here to quantize?

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
                            arpeggiate(arpDirection);
                            currentNoteState = NoteState::IDLE;
                            if (isRestEnabled) break; // comment to remove gap between notes (goes stright into next switch function instead of waiting)
                        } else {
                            NOTE_HANDLING::voice_off(currentVoiceIndex, currentPlayNote, 0);
                            MIDI::sendNoteOff(currentPlayNote, MIDI_DEFAULT_NOTE_OFF_VEL);
                            
                            currentVoiceIndex = (currentVoiceIndex + 1) % POLYPHONY;


                            arpeggiate(arpDirection);
                            currentNoteState = NoteState::IDLE;
                            if (isRestEnabled) break; // comment to remove gap between notes (goes stright into next switch function instead of waiting)
                        }
                        
                    case NoteState::IDLE:
                        if (arpMode == ArpMode::POLY) {
                            for (int i = 0; i < currentNoteCount; i++) {
                                if (arpVoices[i].isActive()) {
                                    currentPlayOct[i] = ((arpVoices[i].play())+(currentOctave*12));

                                    NOTE_HANDLING::voice_on(i, currentPlayOct[i], 127);
                                    MIDI::sendNoteOn(currentPlayOct[i], 127);
                                }
                            }
                            currentNoteState = NoteState::ACTIVE;
                        } else {
                            if (currentPlayIndex >= currentNoteCount) currentPlayIndex = 0;

                            if (arpVoices[currentPlayIndex].isActive()) {
                                currentPlayNote = ((arpVoices[currentPlayIndex].play())+(currentOctave*12));

                                NOTE_HANDLING::voice_on(currentVoiceIndex, currentPlayNote, 127);
                                MIDI::sendNoteOn(currentPlayNote, 127);

                                currentNoteState = NoteState::ACTIVE;
                            }
                            break;
                        }
                }
                CLOCK::set_changed(false);
            }
        }
    }

    
    // Add notes to the arpeggiator input buffer
    void addNote (uint8_t note) {
        if (note == 0) {
            // printf("Wierd, empty addNote...\n"); 
            return;
        }

        if (latchEnabled) {
            // if the chord refresh option is enabled
            if (latchRefresh) { 
                // refresh timeout
                clearAllNotes();
                latchRefresh = false;
                // latchCount = 0; // don't think this is needed with proper removeNotes implementaion
            }   

            ++latchCount;
            
            printf("++latchCount: %d\n", latchCount);

            if (latchCount == MAX_ARP) latchCount = MAX_ARP;
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
                    
                    // return at first free note.
                    return;
                }
            }
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
        volatile uint8_t bufferSize;

        // If inputBuffer is full, use all the notes; if not, use however many there are
        if (inputBufferFull) {
            bufferSize = MAX_ARP;
        } else {
            bufferSize = inputNoteCount;
        }
        
        if (latchEnabled) {
            // new chord type of arp latching - allows you to actually play it without a pedal
            latchCount--;

            printf("--latchCount: %d\n", latchCount);
            
            if (latchCount <= 0) {
                latchCount = 0;
                latchRefresh = true;
            }
        }

        // Check all the active notes
        for (int i = 0; i < bufferSize; ++i) {
            // If the note is here
            if (inputBuffer[i].note == note) {
                if (isSustainEnabled) {
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
                    // dont break incase theres multiple entries 
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
        
        transferNotes();

        currentOctave = 0;
        currentPlayIndex = 0;

        latchCount = 0;
    }

    void passNotes (void) {
        // for passing active notes through to voices when the arp is turned off...
        uint8_t activeVoices;

        if (!isSustainEnabled) {
            // sustain not active - send all actual notes
            for (int i = 0; i < MAX_ARP; i++) {
                // copy all the notes form arp to normal voices
                uint8_t note = arpVoices[i].note;
                if (note != 0) {
                    NOTE_HANDLING::voice_on(activeVoices, note, 127);
                    MIDI::sendNoteOn(note, 127);
                    activeVoices++;
                }
            }
            // update the filter envelope
            NOTE_HANDLING::voices_set(activeVoices);
        } else {
            // sustain is active - only pass held notes, we want the sustained notes to ring out
            for (int i = 0; i < MAX_ARP; i++) {
                uint8_t note = inputBuffer[i].note;
                // check there is a note number, if not, skip it
                if (note == 0)  break;
                
                // pass the note out
                NOTE_HANDLING::voice_on(activeVoices, note, 127);
                MIDI::sendNoteOn(note, 127);

                // mark the note as sustained if it is 
                if (inputBuffer[i].isSustained()) NOTE_HANDLING::note_off(note, MIDI_DEFAULT_NOTE_OFF_VEL); 

                activeVoices++;

            }
            NOTE_HANDLING::voices_set(activeVoices);
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

            printf("grabNotes latchCount: %d\n", latchCount);
            
            if (latchCount <= 0) {
                latchCount = 0;
                latchRefresh = true;
            }
        }
        transferNotes();
    }
    void stopAllVoices () {
        NOTE_HANDLING::voices_clr();
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
    void setRest (uint16_t input) {
        bool temp = (bool)(input >> 9);
        if (isRestEnabled != temp) isRestEnabled = temp;
    }
    void setBpm (uint16_t input) {
        CLOCK::setBpm(map(input, KNOB_MIN, KNOB_MAX, 30, 350));
    }
}