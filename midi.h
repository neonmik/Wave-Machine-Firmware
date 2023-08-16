#pragma once

#include "pico/stdlib.h"

#include "drivers/usb.h"
// #include "drivers/uart.h"

#include "synth/clock.h"
#include "synth/controls.h"


#include "debug.h"

#include "config.h"
#include "functions.h"

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


    namespace {
        uint8_t channel_;

        int recvMode_;
        int recvByteCount_;
        int recvEvent_;
        int recvArg0_;
        int recvBytesNeeded_;
        int lastStatusSent_;
    }
    enum MidiType {
        INVALID_TYPE            = 0x00,    ///< For notifying errors
        NOTE_OFF                = 0x80,    ///< Note Off
        NOTE_ON                 = 0x90,    ///< Note On
        AFTERTOUCH_POLY         = 0xA0,    ///< Polyphonic AfterTouch
        CONTROL_CHANGE          = 0xB0,    ///< Control Change / Channel Mode
        PROGRAM_CHANGE          = 0xC0,    ///< Program Change
        AFTERTOUCH_CHANNEL      = 0xD0,    ///< Channel (monophonic) AfterTouch
        PITCH_BEND              = 0xE0,    ///< Pitch Bend
        SYS_EX                  = 0xF0,    ///< System Exclusive
        TIMECODE_QUATER_FRAME   = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
        SONG_POSITION           = 0xF2,    ///< System Common - Song Position Pointer
        SONG_SELECT             = 0xF3,    ///< System Common - Song Select
        TUNE_REQUEST            = 0xF6,    ///< System Common - Tune Request
        CLOCK                   = 0xF8,    ///< System Real Time - Timing Clock
        START                   = 0xFA,    ///< System Real Time - Start
        CONTINUE                = 0xFB,    ///< System Real Time - Continue
        STOP                    = 0xFC,    ///< System Real Time - Stop
        ACTIVE_SENSING          = 0xFE,    ///< System Real Time - Active Sensing
        SYSTEM_RESET            = 0xFF,    ///< System Real Time - System Reset
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

    
    void init(void);
    void update(void);
    void midi_player(void);

    void sendMidiMessage (uint8_t type, uint8_t channel, uint8_t data1, uint8_t data2);
    void handleMidiMessage(uint8_t msg[4]);

    // Functions for MIDI out
    void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);
    void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
    void handleVelocityChange(uint8_t channel, uint8_t note, uint8_t velocity);
    void handleControlChange(uint8_t channel, uint8_t controller, uint8_t value);
    void handleProgramChange(uint8_t channel, uint8_t program);
    void handleAfterTouch(uint8_t channel, uint8_t velocity);
    void handlePitchBend(uint8_t channel, uint16_t pitch);
    void handleSongPosition(uint8_t position_msb, uint8_t position_lsb);
    void handleSongSelect(uint8_t song);
    void handleTuneRequest(void);
    void handleClock(void);
    void handleStart(void);
    void handleStop(void);
    void handleContinue(void);
    void handleActiveSense(void);
    void handleReset(void);

    // MIDI Out
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

    

}