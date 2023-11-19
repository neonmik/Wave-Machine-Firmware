#pragma once

#include "config.h"
#include "functions.h"

#include "drivers/usb.h"
#include "drivers/uart.h"

#include "synth/clock.h"
#include "controls.h"

namespace MIDI {

    constexpr   uint8_t     MIDI_CHANNEL_OMNI =             0;
    constexpr   uint8_t     MIDI_CHANNEL_OFF =              17; // and over
    
    // This is because MIDI pitchbend is a 14 bit number
    constexpr   uint16_t    MIDI_PITCHBEND_MIN =            -8192;
    constexpr   uint16_t    MIDI_PITCHBEND_MAX =            8191;

    // this is because general MIDI CC messages are 7 bit numbers 
    constexpr   uint8_t     CONTROL_CHANGE_MIN =            0;
    constexpr   uint8_t     CONTROL_CHANGE_MAX =            127;

    constexpr   uint16_t    EXTENDED_CONTROL_CHANGE_MAX =   16384;

    constexpr   uint16_t    SYSEX_MAX_SIZE =                255;


    
    enum MidiType {
        InvalidType           = 0x00,    ///< For notifying errors
        NoteOff               = 0x80,    ///< Channel Message - Note Off
        NoteOn                = 0x90,    ///< Channel Message - Note On
        AfterTouchPoly        = 0xA0,    ///< Channel Message - Polyphonic AfterTouch
        ControlChange         = 0xB0,    ///< Channel Message - Control Change / Channel Mode
        ProgramChange         = 0xC0,    ///< Channel Message - Program Change
        AfterTouchChannel     = 0xD0,    ///< Channel Message - Channel (monophonic) AfterTouch
        PitchBend             = 0xE0,    ///< Channel Message - Pitch Bend
        SystemExclusive       = 0xF0,    ///< System Exclusive
        SystemExclusiveStart  = SystemExclusive,   ///< System Exclusive Start
        TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
        SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
        SongSelect            = 0xF3,    ///< System Common - Song Select
        Undefined_F4          = 0xF4,
        Undefined_F5          = 0xF5,
        TuneRequest           = 0xF6,    ///< System Common - Tune Request
        SystemExclusiveEnd    = 0xF7,    ///< System Exclusive End
        Clock                 = 0xF8,    ///< System Real Time - Timing Clock
        Undefined_F9          = 0xF9,
        Tick                  = Undefined_F9, ///< System Real Time - Timing Tick (1 sampleClockTick = 10 milliseconds)
        Start                 = 0xFA,    ///< System Real Time - Start
        Continue              = 0xFB,    ///< System Real Time - Continue
        Stop                  = 0xFC,    ///< System Real Time - Stop
        Undefined_FD          = 0xFD,
        ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
        SystemReset           = 0xFF,    ///< System Real Time - System Reset
    };

    enum MidiFilterMode
    {
        OFF                     = 0,  ///< Thru disabled (nothing passes through).
        FULL                    = 1,  ///< Fully enabled Thru (every incoming message is sent back).
        SAME_CHANNEL            = 2,  ///< Only the messages on the Input Channel will be sent back.
        DIFFERENT_CHANNEL       = 3,  ///< All the messages but the ones on the Input Channel will be sent back.
    };

    enum MidiControlChangeNumber
    {
        // High resolution Continuous Controllers MSB (+32 for LSB) ----------------
        BANK_SELECT                     = 0,
        MODULATION_WHEEL                = 1,
        BREATH_CONTROLLER               = 2,
        // CC3 undefined
        FOOT_CONTROLLER                 = 4,
        PORTAMENTO_TIME                 = 5,
        DATA_ENTRY                      = 6,
        CHANNEL_VOULME                  = 7,
        BALANCE                         = 8,
        // CC9 undefined
        PAN                             = 10,
        EXPERSSION_CONTROLLER           = 11,
        EFFECT_CONTROL_1                = 12,
        EFFECT_CONTROL_2                = 13,
        // CC14 undefined
        // CC15 undefined
        GENERAL_PURPOSE_CONTROLLER_1    = 16,
        GENERAL_PURPOSE_CONTROLLER_2    = 17,
        GENERAL_PURPOSE_CONTROLLER_3    = 18,
        GENERAL_PURPOSE_CONTROLLER_4    = 19,

        // Switches ----------------------------------------------------------------
        SUSTAIN                         = 64,
        PORTAMENTO                      = 65,
        SOSTENUTO                       = 66,
        SOFT_PEDAL                      = 67,
        LEGATO                          = 68,
        HOLD                            = 69,

        // Low resolution continuous controllers -----------------------------------
        SOUND_CONTROLLER_1              = 70,   ///< Synth: Sound Variation   FX: Exciter On/Off 
        SOUND_CONTROLLER_2              = 71,   ///< Synth: Harmonic Content  FX: Compressor On/Off
        SOUND_CONTROLLER_3              = 72,   ///< Synth: Release Time      FX: Distortion On/Off
        SOUND_CONTROLLER_4              = 73,   ///< Synth: Attack Time       FX: EQ On/Off
        SOUND_CONTROLLER_5              = 74,   ///< Synth: Brightness        FX: Expander On/Off
        SOUND_CONTROLLER_6              = 75,   ///< Synth: Decay Time        FX: Reverb On/Off
        SOUND_CONTROLLER_7              = 76,   ///< Synth: Vibrato Rate      FX: Delay On/Off
        SOUND_CONTROLLER_8              = 77,   ///< Synth: Vibrato Depth     FX: Pitch Transpose On/Off
        SOUND_CONTROLLER_9              = 78,   ///< Synth: Vibrato Delay     FX: Flange/Chorus On/Off
        SOUND_CONTROLLER_10             = 79,   ///< Synth: Undefined         FX: Special Effects On/Off
        GENERAL_PURPOSE_CONTROLLER_5    = 80,
        GENERAL_PURPOSE_CONTROLLER_6    = 81,
        GENERAL_PURPOSE_CONTROLLER_7    = 82,
        GENERAL_PURPOSE_CONTROLLER_8    = 83,
        PORTAMENTO_CONTROL              = 84,
        // CC85 to CC90 undefined
        EFFECTS_1                       = 91,   ///< Reverb send level
        EFFECTS_2                       = 92,   ///< Tremolo depth
        EFFECTS_3                       = 93,   ///< Chorus send level
        EFFECTS_4                       = 94,   ///< Celeste depth
        EFFECTS_5                       = 95,   ///< Phaser depth

        // Channel Mode messages ---------------------------------------------------
        ALL_SOUND_OFF                   = 120,
        RESET_ALL_CONTROLLERS           = 121,
        LOCAL_CONTROL                   = 122,
        ALL_NOTES_OFF                   = 123,
        OMNI_MODE_OFF                   = 124,
        OMNI_MODE_ON                    = 125,
        MONO_MODE_ON                    = 126,
        POLY_MODE_ON                    = 127
    };

    struct MidiMessage {
        MidiType    type = MidiType::InvalidType; // defaults to invalid
        uint8_t     channel;
        uint8_t     data1;
        uint8_t     data2;
        uint8_t     dataSysex[SYSEX_MAX_SIZE];

        uint8_t     length = 0;

        bool        valid;
 
        inline bool isSystemRealTime () const {
            return (type & 0xf8) == 0xf8;
        }
        inline bool isSystemCommon () const {
            return (type & 0xf8) == 0xf0;
        }
        inline bool isChannelMessage () const {
            return (type & 0xf0) != 0xf0;
        }
    };

    namespace {
        
        bool Use1ByteParsing = false;
        bool NullVelocityNoteOnAsNoteOff = true;

        uint16_t    pendingMessageIndex;
        uint16_t    pendingMessageExpectedLength;
        uint8_t     pendingMessage[3];
        
        MidiType    runningStatus;

        MidiMessage inputMessageUART;
        MidiMessage inputMessageUSB;

        bool        sendClockFlag;
    }


    

    void init(void);
    void update(void);

    void checkClockFlag (void);
    void toggleClockFlag (void);

    bool parse (void); // parsing - currently only used for UART
    void resetInput (void);

    bool inputFilter(MidiMessage message);
    void handleNullVelocity(MidiMessage message);
    MidiType getTypeFromStatusByte(uint8_t inStatus);
    uint8_t getChannelFromStatusByte(uint8_t inStatus);
    bool isChannelMessage(MidiType inType);

    // Functions for MIDI out
    void handleNoteOff(uint8_t note, uint8_t velocity);
    void handleNoteOn(uint8_t note, uint8_t velocity);
    void handleVelocityChange(uint8_t note, uint8_t velocity);
    void handleControlChange(uint8_t controller, uint8_t value);
    void handleProgramChange(uint8_t program);
    void handleAfterTouch(uint8_t velocity);
    void handlePitchBend(uint16_t pitch);
    void handleSysEx(MidiMessage message);
    void handleSongPosition(uint8_t position_msb, uint8_t position_lsb);
    void handleSongSelect(uint8_t song);
    void handleTuneRequest(void);
    void handleClock(void);
    void handleStart(void);
    void handleStop(void);
    void handleContinue(void);
    void handleActiveSense(void);
    void handleReset(void);
    void handleInvalidType(MidiMessage message);

    // MIDI Out
    void sendMidiMessage (MidiType type, uint8_t channel, uint8_t *msg);

    // None of these functions have a MIDI channel input as that is controlled at the system level
    void sendNoteOff(uint8_t note, uint8_t velocity);
    void sendNoteOn(uint8_t note, uint8_t velocity);
    void sendVelocityChange(uint8_t note, uint8_t velocity);
    void sendControlChange(uint8_t controller, uint8_t value);
    void sendProgramChange(uint8_t program);
    void sendAfterTouch(uint8_t velocity);
    void sendPitchBend(uint16_t pitch);
    void sendSongPosition(uint8_t position);
    void sendSongSelect(uint8_t song);
    void sendTuneRequest(void);
    void sendClock(void);
    void sendStart(void);
    void sendContinue(void);
    void sendStop(void);
    void sendActiveSense(void);
    void sendReset(void);

    void print (uint8_t *packet);

    void printMidiIn (void);
    void printMidiOut (void);
    void printMidiType (MidiType type);
    void printMidiMessage (MidiMessage message);

}