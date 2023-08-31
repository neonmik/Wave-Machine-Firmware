#pragma once

// #include <stdio.h>
// #include <algorithm>
#include "pico/stdlib.h"

#include "../functions.h"
#include "../config.h"

#include "note_handling.h"
#include "clock.h"


namespace ARP {


    namespace {
        
        enum class NoteState {
            IDLE,
            ACTIVE,
            RELEASE,
        };

        enum class ArpMode {
            MONO,
            PARA,
            POLY,
        };

        enum class ArpDirection {
            UP,
            DOWN,
            UP_DOWN,
            DOWN_UP,
            // RAND,
            // PLAY_ORDER
        };

        ArpDirection    arpDirection = ArpDirection::UP;
        ArpMode         arpMode = ArpMode::POLY;
        NoteState       currentNoteState = NoteState::IDLE;

        bool            isRestEnabled = false;

        bool            isArpActive;

        bool            latchEnabled; // This is used to enable/disable the chord latching feature... only found used in hold/latch not sustain (I think?)
        bool            latchRefresh = false;
        int8_t          latchCount = 0;
        uint32_t        chordRefreshTimeout;
        
        bool            inputBufferFull;
        bool            inputNotesUpdated;
        int8_t          inputNoteCount;
        int8_t          inputWriteIndex;

        uint8_t         currentPlayNote;
        int8_t          currentPlayIndex;
        uint8_t         currentVoiceIndex;
        int8_t          currentNoteCount;

        uint8_t         currentPlayOct[POLYPHONY];
        bool            octavePlaying = false;

        bool            isSustainJustReleased = false;

        bool            isSustainEnabled;
        uint16_t        hold;
        uint16_t        division;
        uint16_t        range;
        uint16_t        direction;

        int8_t          octaveRange;
        int8_t          currentOctave;

        // int8_t          _write_index;
        // bool            _notes_changed = false;

        bool changeDirection = true;
    }

    struct ArpData {
        uint8_t     note; // holds MIDI note number
        uint8_t     velocity;
        bool        sustained;

        void add (uint8_t input) {
            note = input;
            velocity = 127;
            sustained = false; // resets the susainted marker every time the note is re-added
        }

        void sustain (void) {
            sustained = true;
        }

        void remove (void) {
            note = 0;
            velocity = 0;
            sustained = false;
        }

        bool isActive (void) {
            return note > 0;
        }

        bool isSustained (void) {
            return sustained;
        }

        uint8_t play (void) {
            return note;
        }
    };

    extern ArpData arpVoices[MAX_ARP];

    extern ArpData inputBuffer[MAX_ARP];

    

    void Init (void);
    
    void Update(void);
    
    void addNote (uint8_t note);
    void removeNote (uint8_t note);

    void transferNotes (void);

    void clearSustainedNotes (void);
    void clearAllNotes (void);

    void passNotes(void);
    void grabNotes(void);
    void stopAllVoices (void);
    
    void set_state (bool state);
    bool get_state (void);
    void reset (void);

    void setHold (uint16_t input);
    void setDivision (uint16_t input);
    void setRange (uint16_t input);
    void setDirection (uint16_t input);

    void setRest (uint16_t input);
    void setSustain (bool input);
    void setLatch (bool input);

    void updateOctave (bool rising);

    void setBpm (uint16_t input);
}

