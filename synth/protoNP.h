#pragma once

#include "../config.h"

enum class NotePriority {
    FIRST,      // First/oldest notes stay the longest
    LAST,       // Last/newest notes stay the longest
    HIGHEST,    // Highest notes stay the longest
    LOWEST,     // Lowest notes stay the longest
};

enum class Mode {
    POLY,
    MONO,
    ARP,
};

struct VoiceData {
    uint8_t     note;
    uint8_t     velocity;
    bool        gate;
    bool        active;
    uint32_t    activationTime;
    bool        sustained;  // shouldnt be needed as sustain is now handled further up.

    void on (uint8_t _note, uint8_t _velocity) {
        note = _note;
        velocity = _velocity;
        gate = true;
        active = true;
        activationTime = sampleClock;
        sustained = false;
    }
    
    void off (void) {
        gate = false;
    }

    void clear (void) {
        note = 0;
        velocity = 0;
        active = false;
        activationTime = 0;
        sustained = false;
    }
};

namespace Poly {

    namespace {
        NotePriority prioritySetting = NotePriority::LAST;

        uint8_t maxPolyphony = POLYPHONY;
        
        VoiceData voices[maxPolyphony];

        void voiceOn(uint8_t slot, uint8_t note, uint8_t velocity) {
            if (!note) return;

            voices[slot].on(note, velocity);

            QUEUE::triggerSend(slot, voices[slot].note, voices[slot].gate);
        }
        void voiceOff(uint8_t slot, uint8_t note, uint8_t velocity) {

            voices[slot].off();

            QUEUE::triggerSend(slot, voices[slot].note, voices[slot].gate);
        }
    }
    


    void addNote(uint8_t note, uint8_t velocity) {

        NoteLatch::checkLatch();

        // play note
        volatile int8_t voice = -1; // means if no free voices are left, it will be -1 still

        for (int i = 0; i < maxPolyphony; i++)  {
            // voice is being used, but by the same note, so fire again
            if (voices[i].note == note && voices[i].gate) { 
                voice = i;
                break;  // breaks for-loop as a free slot has been found
            }

            // Voice is free
            if (!voices[i].active) {
                voice = i;
                break;
            }
        }

        // should skip this is a free voice is found
        if (voice < 0) {
            int8_t priorityVoice = -1;

            volatile uint32_t timeNow = sampleClock;
            
            switch (prioritySetting) {
                // When there are no free voices on the synth, this code uses various priority types to assign the new note to a voice.
                case Priority::LAST: {
                    // This mode assigns the new note to the voice with the oldest persisting voice (The Latest notes played have prioirty)
                    uint32_t longestReleasedTime = timeNow;
                    uint32_t longestActiveTime = timeNow;
                    for (int i = 0; i < maxPolyphony; i++)  {
                        if (!voices[i].gate && (voices[i].activationTime<longestReleasedTime)) { // released notes
                        longestReleasedTime = voices[i].activationTime;
                        voice = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
                        }
                        else if (voices[i].activationTime<longestActiveTime) { // still active

                        longestActiveTime = voices[i].activationTime;
                        priorityVoice = i; // will give the oldest voice thats still being used
                        }
                    }
                    break; 
                }
                case Priority::FIRST: {
                    // This mode assigns the new note to the newest persisting voice (the first notes played have priority)
                    uint32_t shortestReleasedTime = 0;
                    uint32_t shortestActiveTime = 0;
                    for (int i = 0; i < maxPolyphony; i++)  {
                        // CHANGE!! from active to Gate?
                        if (!voices[i].gate && (voices[i].activationTime>shortestReleasedTime)) { // released notes
                        shortestReleasedTime = voices[i].activationTime;
                        voice = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
                        }
                        else if (voices[i].activationTime>shortestActiveTime) { // still active
                        shortestActiveTime = voices[i].activationTime;
                        priorityVoice = i; // will give the newest slot thats still being used
                        }
                    }
                    break;
                }

                // These modes need checking, and I believe rewritting... probably can remove the active checker.
                // They should only be none active if they make it here, does it need to be gate? I don't think so.
                case Priority::LOWEST: {
                    // This mode assigns the new note only if the new note is lower than one of the persisting notes (the lowest notes have priority)
                    uint8_t lowestNote = 127; // Initialize to the highest possible MIDI note value
                    for (int i = 0; i < maxPolyphony; i++) {
                        if (voices[i].active && voices[i].note < lowestNote) {
                            lowestNote = voices[i].note;
                            voice = i; // Assign the slot with the lowest note
                        }
                    }
                    break;
                }
                case Priority::HIGHEST: {
                    // This mode assigns the new note only if the new note is higher than one of the persisting notes (the highest notes have priority)
                    uint8_t highestNote = 0; // Initialize to the lowest possible MIDI note value
                    for (int i = 0; i < maxPolyphony; i++) {
                        if (voices[i].active && voices[i].note > highestNote) {
                            highestNote = voices[i].note;
                            voice = i; // Assign the slot with the highest note
                        }
                    }
                    break;
                }
            }
            // No slots in release? Use the next priority appropriate active voice
            if (voice < 0) {
                voice = priorityVoice;
            }
        }

        if (sustainPedalActive) {
            // if a voice is allocated while sustain is pressed, but not released, the sustain should be reset until its released again, this should stop current notes being released on the pedal being released. 
            // I think this should apply to every voice? not just new voices... 
            voices[voice].sustained = false;
        }
        voiceOn(voice, note, velocity);
    }

    void removeNote (uint8_t note, uint8_t velocity) {
        for (int8_t voice = 0; voice < POLYPHONY; voice++)  {
            if (voices[voice].note == note)  {
                voiceOff(voice, note, velocity);
            }
        }
    }

    void releaseAllNotes (void) {
        for (int i = 0; i < maxPolyphony; i++) {
            if (voices[i].active) {
                voiceOff(i, 0, 0);
            }
        }
    }
};

namespace Arp {

    void setState(bool state) {
        if (isArpActive == state) return; // If state is already set, do nothing
        isArpActive = state;
        reset();  
    }

    // Returns the state of the arp
    bool getState(void) {
        return isArpActive;
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

    void update (void) {
        // always update clock, will be used for MIDI clock out.
        CLOCK::update();
        // if arp is on, do arp stuff.
        if (isArpActive) {
            uint32_t currentNoteTick = CLOCK::getClockTick();
            if (currentNoteTick >= gate && currentNoteState == NoteState::ACTIVE) {
                currentNoteState = NoteState::RELEASE;
                releaseNote();
            }
            if (CLOCK::getClockChanged()) {
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

    // Add a note to the arpeggiator input buffer
    void addNote (uint8_t note, uint8_t velocity) {
        // Check if the note is valid
        if (note == 0) return; // Invalid note, nothing to do

        NoteLatch::checkLatch();

        // Check if the note already exists in the inputBuffer - this has been removed so that I can do double notes in longer sequences
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

        // Iterate through the active notes in the buffer
        for (int i = 0; i < bufferSize; ++i) {
            // Check if the note is in the buffer
            if (inputBuffer[i].note == note) {
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

    void transferNotes () {
        if (inputNotesUpdated) {
            
            // printNoteBuffer(inputBuffer);

            memcpy(playBuffer, inputBuffer, sizeof(inputBuffer));

            uint8_t length      = inputNoteCount;   // how many entries in the array

            if (!playedOrder) { // Allows the notes to not be organised for Played Order mode, this should still mean all the zeros are at the top... check!
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

}

namespace NoteLatch {

    namespace {
        bool        latchEnabled = false;
        bool        refreshLatchedNotes = false;
        bool        releaseNotes = false;

        uint8_t     latchedNotes = 0;
        uint8_t     maxLatchedNotes = 8;
    }
    
    void setState (bool state) {
        latchEnabled = state;
    }
    void toggleState (void) {
        latchEnabled = !latchEnabled;
    }
    bool getState (void) {
        return latchEnabled;
    }

    void setMaxLatchedNotes (uint8_t max) {
        maxLatchedNotes = max;
    }

    bool increaseCount (void) {
        releaseNotes = false;

        // add note to latch
        if (!latchEnabled) return false;

        if (refreshLatchedNotes) {
            refreshLatchedNotes = false;
            releaseNotes = true;
        }

        ++latchedNotes;

        if (latchedNotes >= maxLatchedNotes) {
            latchedNotes = maxLatchedNotes; // Cap the count
        }

        return releaseNotes;
    }
    void decreaseCount (void) {
        // remove note from latch
        if (!latchEnabled) return;
        // Handle latch behavior
        if (latchedNotes > 0) {
            latchedNotes--;

            if (latchedNotes == 0) {
                refreshLatchedNotes = true;
            }
        }
    }

    bool resetNotes (void) {
        if (releaseNotes) {
            releaseNotes = false;
            return true;
        } else {
            return false;
        }
    }

    void resetCount (void) {
        latchedNotes = 0;
        refreshLatchedNotes = false;
    }
}

namespace NoteSustain {
    namespace {
        bool        sustainEnabled = false;

        bool        releaseSustainedNotes = false;

        uint8_t     noteList[128];
        uint8_t     sustainedNotes = 0;

        void increaseCount (void) {
            if (sustainedNotes < 127) {
                ++sustainedNotes;
            }
        }
        void decreaseCount (void) {
            if (sustainedNotes > 0) {
                --sustainedNotes;
            }
        }
    }


    void setState (bool state) {
        if (sustainEnabled == state) return;

        sustainEnabled = state;

        if (!sustainEnabled) {
            releaseSustainedNotes = true;
        }
    }
    void toggleState (void) {
        sustainEnabled = !sustainEnabled;

        if (!sustainEnabled) {
            releaseSustainedNotes = true;
        }
    }
    bool getState (void) {
        return sustainEnabled;
    }

    void addNote (uint8_t note) {
        if (!note) return;

        if (sustainedNotes) {
            // check if note is already in the list
            for (uint8_t i = 0; i < sustainedNotes; i++) {
                if (noteList[i] == note) return;
            }
        }

        noteList[sustainedNotes] = note;

        increaseCount();
    }
    uint8_t releaseNote (void) {
        if (!sustainedNotes) return 0;

        uint8_t note = noteList[sustainedNotes];

        decreaseCount();

        return noteList[note];
    }
}

namespace NoteHandling {

    namespace {
        Mode mode = Mode::POLY;
        
        bool releaseSustainedNotes = false;

        bool releaseLatchedNotes = false;
    }


    void playNote (uint8_t note, uint8_t velocity) {
        NoteLatch::increaseCount();
        if (NoteLatch::resetNotes()) {
            // release and clear all latched notes before adding new one

            // clear all notes
            switch(mode) {
                case Mode::POLY:        Poly::releaseAllNotes();      break;
                case Mode::MONO:        Mono::releaseAllNotes();      break;
                case Mode::ARP:         ARP::releaseAllNotes();       break;
            }
        }

        // play note
        switch(mode) {
            case Mode::POLY:            Poly::addNote(note, velocity);      break;
            case Mode::MONO:            Mono::addNote(note, velocity);      break;
            case Mode::ARP:             ARP::addNote(note, velocity);       break;
        }
    }

    void releaseNote (uint8_t note, uint8_t velocity) {
        NoteLatch::decreaseCount();
        // release note
        switch(mode) {
            case Mode::POLY:            Poly::releaseNote(note, velocity);      break;
            case Mode::MONO:            Mono::releaseNote(note, velocity);      break;
            case Mode::ARP:             ARP::releaseNote(note, velocity);        break;
        }
    }

    
    void noteOn (uint8_t note, uint8_t velocity, bool passMidi = true) {
        // play note
        playNote(note, velocity);
        if (!passMidi) MIDI::sendNoteOn(note, velocity);

    }

    void noteOff (uint8_t note, uint8_t velocity, bool passMidi = true) {
        if (NoteSustain::getState()) {
            // do not release note - add it to the sustain list
            NoteSustain::addNote(note);
        } else {
            // release note
            releaseNote(note, velocity);
            if (!passMidi) MIDI::sendNoteOn (note, velocity);
        }
    }

    void update (void) {
        
        checkReleaseMessages(); // needs to happen outside of any specific note handling mode

        releaseSustainedNotes(); // handles the release of sustained notes as and when it needs to happens
        
        ARP::update();
    }

    // add a way of tracking hardware sustain pedal on start up - could be within Keys class
    void setSustainPedal (uint16_t value) {
        // set sustain pedal state
        bool status = (value >> 9);

        NoteSustain::setSustain(status);
            
        if (!NoteSustain::getState()) releaseSustainedNotes = true;



        // Code for Arp Latch:
        if (ARP::getState()) {
            // this code is acts like a momentary switch, only latching when held
            // ARP::toggleSustain(); 
            
            // this code toggles the Latch per press.
            if (NoteSustain::getSustain()) {
                ARP::toggleSustain(); 
            }
        }
    }

    void releaseSustainedNotes (void) {
        if (releaseSustainedNotes) {
            if (uint8_t temp = NoteSustain::releaseNote()) {
                releaseNote(note, 0);
                if (!passMidi) MIDI::sendNoteOn (note, 0);
            }
            releaseSustainedNotes = false;
        }
    }

    void checkReleaseMessages () {
        uint8_t queueLength = QUEUE::releaseCheckQueue();
        if (queueLength) {
        for (int i = 0; i < queueLength; i++) {
            // receives the slot number thats released from the queue, and then clears the slot on this core.
            uint8_t slot = QUEUE::releaseReceive();
            voices[slot].clear();
        }
        }
    }
    
};
