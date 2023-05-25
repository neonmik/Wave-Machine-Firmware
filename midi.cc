#include "midi.h"

namespace MIDI {
    
    void processMidiMessage(uint8_t msg[4]) {
        uint8_t midiType, channel, data1, data2;
        if (msg[1] >= 0xF0) { // check if SYS EX, as they use the lower half of the status byte
            midiType = msg[1]; // Extract the MIDI type
            data1 = msg[2]; // Extract the first data byte
            data2 = msg[3]; // Extract the second data byte
        } else { // everything else uses it for channel data
            midiType = msg[1] & 0xF0; // Extract the MIDI type
            channel = msg[1] & 0x0F; // Extract the channel from the status byte
            data1 = msg[2]; // Extract the first data byte
            data2 = msg[3]; // Extract the second data byte
        }

        switch (midiType) {
            // case MidiType::INVALID_TYPE:
            //     printf("Invalid MIDI Message type!\n");
            //     break;
            case MidiType::NOTE_OFF:
                handleNoteOff(channel, data1, data2);
                break;
            case MidiType::NOTE_ON:
                handleNoteOn(channel, data1, data2);
                break;
            case MidiType::AFTERTOUCH_POLY:
                // Handle Polyphonic AfterTouch message
                // Call the corresponding handler function
                break;
            case MidiType::CONTROL_CHANGE:
                handleControlChange(channel, data1, data2);
                break;
            case MidiType::PROGRAM_CHANGE:
                // Handle Program Change message
                // Call the corresponding handler function
                break;
            case MidiType::AFTERTOUCH_CHANNEL:
                break;
            case MidiType::PITCH_BEND:
            case MidiType::SYS_EX:
                break;
            case MidiType::SONG_POSITION:
                break;
            case MidiType::SONG_SELECT:
                break;
            case MidiType::TUNE_REQUEST:
                // handleTuneRequest();
                break;
            case MidiType::CLOCK:
                handleClock();
                break;
            case MidiType::START:
                // handleStart();
                break;
            case MidiType::CONTINUE:
                // handleContinue();
                break;
            case MidiType::STOP:
                // handleStop();
                break;
            case MidiType::ACTIVE_SENSING:
                // handleActiveSense();
                break;
            case MidiType::SYSTEM_RESET:
                // handleReset();
                break;
            // Handle other MIDI types
            default:
                break;
        }
    }

        //  MIDI Callbacks
    void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
        // printf("MIDI IN: NOTE OFF!\n");
        if (channel == MIDI_CHANNEL) KEYS::note_off(note);
    }

    void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
        // printf("MIDI IN: NOTE ON!\n");
        if (channel == MIDI_CHANNEL && velocity > 0) KEYS::note_on(note);
    }

    void handleClock(void) {
        // printf("MIDI IN: CLOCK!\n");
        BEAT_CLOCK::midi_tick();
    }

    void handleStart(void) {
        // pp6_set_midi_start();
    }

    void handleStop(void) {
        // pp6_set_midi_stop();
    }


    void handleVelocityChange(uint8_t channel, uint8_t note, uint8_t velocity) {}

    void handleControlChange(uint8_t channel, uint8_t controller, uint8_t value) {
        uint16_t temp = (value << 3);
        switch (controller) {
            case 1: // Modulation wheel
                break;
            case 7: // Volume
                break;
            case 70: // Wavetable
                // SETTINGS::Control.set(0 , 0, temp);
                break;
            case 71: // Vector
                // SETTINGS::Control.set(0 , 1, temp);
                break;
            case 72: // Release
                // SETTINGS::Control.set(1 , 3, temp);
                break;
            case 73: // Attack
                // SETTINGS::Control.set(1 , 0, temp);
                break;
            case 75: // Decay
                // SETTINGS::Control.set(1 , 1, temp);
                break;


        }
        //midi_cc[(controller - 10) & 0x7] = (float32_t)value / 127.f;
    }

    void handleProgramChange(uint8_t channel, uint8_t program) {}
    void handleAfterTouch(uint8_t channel, uint8_t velocity) {}
    void handlePitchBend(uint8_t pitch) {}
    void handleSongPosition(uint8_t position) {}
    void handleSongSelect(uint8_t song) {}
    void handleTuneRequest(void) {}
    void handleContinue(void) {}
    void handleActiveSense(void) {}
    void handleReset(void) {}

    void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
        printf("MIDI OUT: NOTE OFF!\n");
        uint8_t msg[3] = { (MidiType::NOTE_OFF | channel), note, 0};
        tud_midi_stream_write(USB_MIDI_CABLE_NUMBER, msg, 3);
    }
    void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
        printf("MIDI OUT: NOTE ON!\n");
        uint8_t msg[3] = { MidiType::NOTE_ON | channel, note, 127};
        tud_midi_stream_write(USB_MIDI_CABLE_NUMBER, msg, 3);
    }

    void init () {

        tusb_init();
        printf("\nMIDI INIT\n");
    }

    void usb_midi_task (void) {
        if (!tud_midi_available()) return;

        uint8_t packet[4];
        tud_midi_packet_read(packet);
        processMidiMessage(packet);
    }

    void update () {
        tud_task(); // tinyusb device task
        usb_midi_task();
    }

    //--------------------------------------------------------------------+
    // MIDI Task
    //--------------------------------------------------------------------+

    // Variable that holds the current position in the sequence.
    uint32_t note_pos = 0;

    // Store example melody as an array of note values
    uint8_t note_sequence[] =
    {
    74,78,81,86,90,93,98,102,57,61,66,69,73,78,81,85,88,92,97,100,97,92,88,85,81,78,
    74,69,66,62,57,62,66,69,74,78,81,86,90,93,97,102,97,93,90,85,81,78,73,68,64,61,
    56,61,64,68,74,78,81,86,90,93,98,102
    };

    void midi_player () {
        static uint32_t start_ms = 0;

        uint8_t const channel   = 0; // 0 for channel 1

        // The MIDI interface always creates input and output port/jack descriptors
        // regardless of these being used or not. Therefore incoming traffic should be read
        // (possibly just discarded) to avoid the sender blocking in IO
        uint8_t packet[4];
        while ( tud_midi_available() ) tud_midi_packet_read(packet);

        // send note periodically
        if (board_millis() - start_ms < 286) return; // not enough time
        start_ms += 286;

        // Previous positions in the note sequence.
        int previous = (int) (note_pos - 1);

        // If we currently are at position 0, set the
        // previous position to the last note in the sequence.
        if (previous < 0) previous = sizeof(note_sequence) - 1;

        // Send Note On for current position at full velocity (127) on channel 1.
        uint8_t note_on[3] = { 0x90 | channel, note_sequence[note_pos], 127 };
        tud_midi_stream_write(USB_MIDI_CABLE_NUMBER, note_on, 3);

        // Send Note Off for previous note.
        uint8_t note_off[3] = { 0x80 | channel, note_sequence[previous], 0};
        tud_midi_stream_write(USB_MIDI_CABLE_NUMBER, note_off, 3);

        // Increment position
        note_pos++;

        // If we are at the end of the sequence, start over.
        if (note_pos >= sizeof(note_sequence)) note_pos = 0;
    }
}