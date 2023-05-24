#include "midi.h"

namespace MIDI {
    
    void recvByte(uint8_t &byte) {
        int tmp;
        int channel;
        int bigval;           /*  temp 14-bit value for pitch, song pos */


        // if (recvMode_ & MODE_PROPRIETARY && byte != STATUS_END_PROPRIETARY) {
        //     /* If proprietary handling compiled in, just pass all data received
        //     *  after a START_PROPRIETARY event to proprietary_decode
        //     *  until get an END_PROPRIETARY event
        //     */
        //     #ifdef CONFIG_MIDI_PROPRIETARY
        //             proprietaryDecode(byte);
        //     #endif
        //     return;
        // }

        if (byte & 0x80) {

            /* All < 0xf0 events get at least 1 arg byte so
            *  it's ok to mask off the low 4 bits to figure
            *  out how to handle the event for < 0xf0 events.
            */

            tmp = byte;

            if (tmp < 0xf0)
                tmp &= 0xf0;

                switch (tmp) {
                    /* These status events take 2 bytes as arguments */
                    case MidiType::NOTE_OFF:
                    case MidiType::NOTE_ON:
                    case MidiType::AFTERTOUCH_POLY:
                    case MidiType::CONTROL_CHANGE:
                    case MidiType::PITCH_BEND:
                    case MidiType::SONG_POSITION:
                        recvBytesNeeded_ = 2;
                        recvByteCount_ = 0;
                        recvEvent_ = byte;
                        break;

                    /* 1 byte arguments */
                    case MidiType::PROGRAM_CHANGE:
                    case MidiType::AFTERTOUCH_CHANNEL:
                    case MidiType::SONG_SELECT:
                        recvBytesNeeded_ = 1;
                        recvByteCount_ = 0;
                        recvEvent_ = byte;
                        return;

                    /* No arguments ( > 0xf0 events) */
                    // case STATUS_START_PROPRIETARY:
                    //     recvMode_ |= MODE_PROPRIETARY;
                    //     #ifdef CONFIG_MIDI_PROPRIETARY
                    //                     proprietaryDecodeStart();
                    //     #endif
                    //     break;
                    // case STATUS_END_PROPRIETARY:
                    //     recvMode_ &= ~MODE_PROPRIETARY;
                    //     #ifdef CONFIG_MIDI_PROPRIETARY
                    //                     proprietaryDecodeEnd();
                    //     #endif
                    //     break;
                    case MidiType::TUNE_REQUEST:
                        handleTuneRequest();
                        break;
                    case MidiType::CLOCK:
                        handleClock();
                        break;
                    case MidiType::START:
                        handleStart();
                        break;
                    case MidiType::CONTINUE:
                        handleContinue();
                        break;
                    case MidiType::STOP:
                        handleStop();
                        break;
                    case MidiType::ACTIVE_SENSING:
                        handleActiveSense();
                        break;
                    case MidiType::SYSTEM_RESET:
                        handleReset();
                        break;
                }

            return;
        }

        if (++recvByteCount_ == recvBytesNeeded_) {
            /* Copy out the channel (if applicable; in some cases this will be meaningless,
            *  but in those cases the value will be ignored)
            */
            channel = (recvEvent_ & 0x0f) + 1;

            tmp = recvEvent_;
            if (tmp < 0xf0) {
                tmp &= 0xf0;
            }

            /* See if this event matches our MIDI channel
            *  (or we're accepting for all channels)
            */
            if (!channel_ || (channel == channel_) || (tmp >= 0xf0)){
                switch (tmp) {
                    case MidiType::NOTE_ON:
                        /* If velocity is 0, it's actually a note off & should fall thru
                        *  to the note off case
                        */
                        if (byte) {
                            handleNoteOn(channel, recvArg0_, byte);
                            break;
                        }

                    case MidiType::NOTE_OFF:
                        handleNoteOff(channel, recvArg0_, byte);
                        break;
                    case MidiType::AFTERTOUCH_POLY:
                        handleVelocityChange(channel, recvArg0_, byte);
                        break;
                    case MidiType::CONTROL_CHANGE:
                        handleControlChange(channel, recvArg0_, byte);
                        break;
                    case MidiType::PROGRAM_CHANGE:
                        handleProgramChange(channel, byte);
                        break;
                    case MidiType::AFTERTOUCH_CHANNEL:
                        handleAfterTouch(channel, byte);
                        break;
                    case MidiType::PITCH_BEND:
                        bigval = (byte << 7) | recvArg0_;
                        handlePitchBend(bigval);
                        break;
                    case MidiType::SONG_POSITION:
                        bigval = (byte << 7) | recvArg0_;
                        handleSongPosition(bigval);
                        break;
                    case MidiType::SONG_SELECT:
                        handleSongSelect(byte);
                        break;
                }
            }

            /* Just reset the byte count; keep the same event -- might get more messages
                trailing from current event.
            */
            recvByteCount_ = 0;
        }

        recvArg0_ = byte;
    }

        //  MIDI Callbacks
    void handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity) {
        KEYS::note_on(note);
    }

    void handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity) {
        KEYS::note_off(note);
    }

    void handleClock(void) {
        BEAT_CLOCK::midi_tick();
        // pp6_midi_clock_tick();
    }

    void handleStart(void) {
        // pp6_set_midi_start();
    }

    void handleStop(void) {
        // pp6_set_midi_stop();
    }


    void handleVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity) {}

    void handleControlChange(unsigned int channel, unsigned int controller, unsigned int value) {
        //midi_cc[(controller - 10) & 0x7] = (float32_t)value / 127.f;
    }

    void handleProgramChange(unsigned int channel, unsigned int program) {}
    void handleAfterTouch(unsigned int channel, unsigned int velocity) {}
    void handlePitchChange(unsigned int pitch) {}
    void handleSongPosition(unsigned int position) {}
    void handleSongSelect(unsigned int song) {}
    void handleTuneRequest(void) {}
    void handleContinue(void) {}
    void handleActiveSense(void) {}
    void handleReset(void) {}
}