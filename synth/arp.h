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

        enum class ArpDirection {
            UP,
            DOWN,
            UP_DOWN,
            DOWN_UP,
            // RAND,
            // PLAY_ORDER
        };

        ArpDirection arpDirection = ArpDirection::UP;



        NoteState currentNoteState = NoteState::IDLE;

        bool isRestEnabled = false;

        bool  isArpActive;
        
        bool    inputBufferFull;
        int8_t  inputNoteCount;
        int8_t  inputWriteIndex;

        bool    inputNotesUpdated;

        uint8_t     currentPlayNote;
        int8_t      currentPlayIndex;
        uint8_t     currentVoiceIndex;


        int8_t      currentNoteCount;
        int8_t      _write_index;
        bool        _notes_changed = false;

        bool        isSustainJustReleased = false;

        bool        isHoldEnabled;
        uint16_t    _last_hold;
        uint16_t    _last_division;
        uint16_t    _last_range;
        uint16_t    _last_direction;

        int8_t _range;
        int8_t currentOctave;

        bool _switch = true;
    }

    struct ArpData {
        uint8_t     note; // holds MIDI note number
        uint8_t     velocity;
        bool        sustained;

        void add (uint8_t input) {
            note = input;
            velocity = 127;
            sustained = false; // resets the susainted marker every time the note is added
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
    void clearNote (uint8_t slot);
    void organise_notes (void);
    void clear_all_notes (void);
    void pass_notes(void);
    void grab_notes(void);
    void stop_all (void);
    
    void set_state (bool state);
    bool get_state (void);
    void reset (void);

    void setHold (uint16_t hold);
    void setDivision (uint16_t division);
    void setRange (uint16_t range);
    void setDirection (uint16_t direction);

    void setRest (uint16_t gap);
    void setSustain (bool sus);

    void updateOctave (bool rising);
    // void update_controls (void);

    void setBpm (uint16_t bpm);
}

