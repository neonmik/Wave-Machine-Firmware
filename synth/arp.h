#pragma once

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
        
        enum class OctaveDirection {
            UP,
            DOWN,
        };

        enum class OctaveMode {
            OLD, // Octave jumps always go up, on UP/DOWN settings the arp goes Up then Down, then changes upwards
            NEW, // Juno style. Octave changes follow the direction - on DOWN starts at the top then works backwards, on UP/DOWN goes all the way to the top before coming back down.
        };

        ArpDirection    arpDirection = ArpDirection::UP;
        ArpMode         arpMode = ArpMode::MONO;

        ArpDirection    octaveDirection = ArpDirection::UP;
        OctaveMode      octaveMode = OctaveMode::OLD;

        NoteState       currentNoteState = NoteState::IDLE;

        bool            isRestEnabled = false;

        bool            isArpActive;

        bool            latchEnabled = true; // This is only used to enable/disable the chord latching feature within sustain
        bool            latchRefresh = false;
        int8_t          latchCount = 0;
        uint32_t        chordRefreshTimeout;
        
        bool            inputBufferFull;
        bool            inputNotesUpdated;
        int8_t          inputNoteCount;
        int8_t          inputWriteIndex;

        uint8_t         currentPlayNote;
        int8_t          currentPlayIndex;

        inline void     resetPlayIndex (void) {
            currentPlayIndex = 0;
        }
        
        bool            refreshPlayIndex = false;
        uint8_t         currentVoiceIndex;
        int8_t          currentNoteCount;


        int8_t          octaveRange;
        int8_t          currentOctave;

        inline void     resetOctave (void) {
            currentOctave = 0;
        }
        uint8_t         startOctave;
        uint8_t         currentPlayOct[POLYPHONY];
        bool            octavePlaying = false;

        bool            isSustainJustReleased = false;

        bool            isSustainEnabled;
        bool            isHoldEnabled;

        uint16_t        hold;
        uint16_t        division;
        uint16_t        range;
        uint16_t        direction;


        bool            polyMode = false;


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
    
    void update(void);
    
    void addNote (uint8_t note);
    void removeNote (uint8_t note);

    void transferNotes (void);

    void clearSustainedNotes (void);
    void clearAllNotes (void);

    void passNotes(void);
    void grabNotes(void);
    void stopAllVoices (void);
    
    void setState (bool state);
    bool getState (void);
    void reset (void);

    void setGate (uint16_t input);
    void setDivision (uint16_t input);
    void setRange (uint16_t input);
    void setDirection (uint16_t input);

    void toggleHold (void);
    void toggleSustain (void);
    void updateSustain (void);

    void updateOctave (ArpDirection direction);

    void setBPM (uint16_t input);
    
    void setOctMode (uint16_t input);
}

