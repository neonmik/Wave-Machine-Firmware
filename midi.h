#pragma once

#include "pico/stdlib.h"

#include "drivers/keys.h"
#include "synth/beat_clock.h"


typedef uint8_t byte;

namespace MIDI {

    constexpr   uint8_t     MIDI_CHANNEL_OMNI =     0;
    constexpr   uint8_t     MIDI_CHANNEL_OFF =      17; // and over
    
    // I think this is because MIDI pitchbend is a 14 bit number?
    constexpr   uint16_t    MIDI_PITCHBEND_MIN =    -8192;
    constexpr   uint16_t    MIDI_PITCHBEND_MAX =    8191;

    typedef byte StatusByte;
    typedef byte DataByte;
    typedef byte Channel;
    typedef byte FilterMode;

    namespace {
        uint8_t channel_;

        int recvMode_;
        int recvByteCount_;
        int recvEvent_;
        int recvArg0_;
        int recvBytesNeeded_;
        int lastStatusSent_;
    }
    enum MidiType
    {
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

    // void usb_midi_task() {
    //     if (tud_midi_available() < 4) return;

    //     uint8_t buff[4];

    //     if (tud_midi_packet_read(buff)) {
    //         if (buff[1] == (0x90 | (MIDI_CHANNEL-1))) {
    //             if (buff[3] > 0) {
    //                 note_priority(buff[1], buff[2], buff[3]);
    //             } 
    //         }

    //         if (buff[1] == (0x80 | (MIDI_CHANNEL-1))) {
    //             note_priority(buff[1], buff[2], 0); // (0 on the end as velocity is off)
    //         }

    //         if (buff[1] == (0xE0 | (MIDI_CHANNEL-1))) {
    //             midi_pitch_bend = buff[2] | (buff[3]<<7);
    //         }
    //     }
    // }

    

    // Functions for MIDI out
    void sendNoteOff(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendControlChange(unsigned int channel, unsigned int controller, unsigned int value);
    void sendProgramChange(unsigned int channel, unsigned int program);
    void sendAfterTouch(unsigned int channel, unsigned int velocity);
    void sendPitchBend(unsigned int pitch);
    void sendSongPosition(unsigned int position);
    void sendSongSelect(unsigned int song);
    void sendTuneRequest(void);
    void sendClock(void);
    void sendStart(void);
    void sendContinue(void);
    void sendStop(void);
    void sendActiveSense(void);
    void sendReset(void);

    // Functions for MIDI in
    void handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity);
    void handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);
    void handleVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity);
    void handleControlChange(unsigned int channel, unsigned int controller, unsigned int value);
    void handleProgramChange(unsigned int channel, unsigned int program);
    void handleAfterTouch(unsigned int channel, unsigned int velocity);
    void handlePitchBend(unsigned int pitch);
    void handleSongPosition(unsigned int position);
    void handleSongSelect(unsigned int song);
    void handleTuneRequest(void);
    void handleClock(void);
    void handleStart(void);
    void handleContinue(void);
    void handleStop(void);
    void handleActiveSense(void);
    void handleReset(void);


}