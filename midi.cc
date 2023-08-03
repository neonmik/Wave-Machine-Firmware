#include "midi.h"

namespace MIDI {

    void handleMidiMessage(uint8_t msg[4]) {
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
            // at the top to give priority for timing
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
            
            case MidiType::NOTE_OFF:
                handleNoteOff(channel, data1, data2);
                break;
            case MidiType::NOTE_ON:
                handleNoteOn(channel, data1, data2);
                break;
            case MidiType::AFTERTOUCH_POLY:
                handleVelocityChange(channel, data1, data2);
                break;
            case MidiType::CONTROL_CHANGE:
                handleControlChange(channel, data1, data2);
                break;
            case MidiType::PROGRAM_CHANGE:
                handleProgramChange(channel, data1);
                break;
            case MidiType::AFTERTOUCH_CHANNEL:
                handleAfterTouch(channel, data1);
                break;
            case MidiType::PITCH_BEND:
                 handlePitchBend(channel, ((data2 << 7) | (data1)));
                break;
            case MidiType::SYS_EX:
                break;
            case MidiType::SONG_POSITION: 
                handleSongPosition(data1, data2);
                break;
            case MidiType::SONG_SELECT:
                handleSongSelect(data1);
                break;
            case MidiType::TUNE_REQUEST:
                handleTuneRequest();
                break;
            case MidiType::ACTIVE_SENSING:
                handleActiveSense();
                break;
            case MidiType::SYSTEM_RESET:
                handleReset();
                break;
            case MidiType::INVALID_TYPE:
                // shouldn't happen very often, if at all. 
                // printf("Invalid MIDI Message type!\n");
                break;
            default:
                break;
        }
    }
    void sendMidiMessage (uint8_t type, uint8_t channel, uint8_t data1, uint8_t data2) {
        uint8_t status = (type | channel);
        uint8_t msg[3] = { status, data1, data2 };
        USB::MIDI::send(msg);
        // UART::MIDI::send(msg);
    }
    //  MIDI Callbacks
    void handleNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) {
        if (channel == MIDI_CHANNEL) NOTE_HANDLING::note_off(note, velocity);
    }
    void handleNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
        if (channel == MIDI_CHANNEL) {
            if (velocity > 0) NOTE_HANDLING::note_on(note, velocity);
            else NOTE_HANDLING::note_off(note, velocity);
        }
    }
    void handleVelocityChange(uint8_t channel, uint8_t note, uint8_t velocity) {
        if (channel != MIDI_CHANNEL) return;
    }
    void handleControlChange(uint8_t channel, uint8_t controller, uint8_t value) {
        if (channel != MIDI_CHANNEL) return;
        // add 14-bit midi handling code here too...
        // if ((CC < 64) && ((CC + 32)>0)) 14bit_mode = true;
        // if (14bit_mode) uint16_t temp = map(value, CONTROL_CHANGE_MIN, EXTENDED_CONTROL_CHANGE_MAX, KNOB_MIN, KNOB_MAX);
        uint16_t temp = map(value, CONTROL_CHANGE_MIN, CONTROL_CHANGE_MAX, KNOB_MIN, KNOB_MAX);
        switch (controller) {
            case 1: // Modulation wheel
                // printf("MIDI IN: Modulation Wheel - %d\n", temp);
                break;
            case 7: // Volume
                // printf("MIDI IN: Volume - %d\n", temp);
                break;
            case 70: // Wavetable
                // printf("MIDI IN: Wavetable - %d\n", temp);
                CONTROLS::set_value(CONTROLS::Controls::MAIN, 0, temp);
                break;
            case 71: // Vector
                // printf("MIDI IN: Vector - %d\n", temp);
                CONTROLS::set_value(CONTROLS::Controls::MAIN, 1, temp);
                break;
            case 72: // Release
                // printf("MIDI IN: Release - %d\n", temp);
                CONTROLS::set_value(CONTROLS::Controls::ADSR, 3, temp);
                break;
            case 73: // Attack
                // printf("MIDI IN: Attack - %d\n", temp);
                CONTROLS::set_value(CONTROLS::Controls::ADSR, 0, temp);
                break;
            case 75: // Decay
                // printf("MIDI IN: Decay - %d\n", temp);
                CONTROLS::set_value(CONTROLS::Controls::ADSR, 1, temp);
                break;
            case 64: // Sustain pedal
                // CONTROLS::set_value(CONTROLS::Controls::ARP, 0, temp); 
                NOTE_HANDLING::sustain_pedal(temp);
                break;
            default:
                break;

        }
    }
    void handleProgramChange(uint8_t channel, uint8_t program) {
        if (channel != MIDI_CHANNEL) return;
    }
    void handleAfterTouch(uint8_t channel, uint8_t velocity) {
        if (channel != MIDI_CHANNEL) return;
    }
    void handlePitchBend(uint8_t channel, uint16_t pitch) {
        if (channel != MIDI_CHANNEL) return;
        // printf("MIDI IN: Pitch Bend - %d\n", pitch);
        // uint16_t temp = map(pitch, 0, EXTENDED_CONTROL_CHANGE_MAX, 0, 1023);
        uint16_t temp = pitch >> 4; // easy way to map 14 bit range to the needed internal 10 bit.
        // printf("MIDI IN: Pitch Bend (mapped) - %d\n", temp);
        CONTROLS::set_value(0, 3, temp);
    }
    void handleSongPosition(uint8_t position_msb, uint8_t position_lsb) {}
    void handleSongSelect(uint8_t song) {}
    void handleTuneRequest(void) {}
    void handleClock(void) {
        BEAT_CLOCK::midi_tick();
    }
    void handleStart(void) {
        BEAT_CLOCK::start_midi_clock();
    }
    void handleStop(void) {
        BEAT_CLOCK::stop_midi_clock();
    }
    void handleContinue(void) {}
    void handleActiveSense(void) {}
    void handleReset(void) {}

    // None of these functions have a MIDI channel input as that is controlled at the system level
    void sendNoteOff(uint8_t note, uint8_t velocity) {
        sendMidiMessage(MidiType::NOTE_OFF, MIDI_CHANNEL, note, velocity);
    }
    void sendNoteOn(uint8_t note, uint8_t velocity) {
        sendMidiMessage(MidiType::NOTE_ON, MIDI_CHANNEL, note, velocity);
    }
    void sendVelocityChange(uint8_t note, uint8_t velocity) {
        sendMidiMessage(MidiType::AFTERTOUCH_POLY, MIDI_CHANNEL, note, velocity);
    }
    void sendControlChange(uint8_t controller, uint8_t value){
        sendMidiMessage(MidiType::CONTROL_CHANGE, MIDI_CHANNEL, controller, value);
    }
    void sendProgramChange(uint8_t program) {
        sendMidiMessage(MidiType::PROGRAM_CHANGE, MIDI_CHANNEL, program, 0);
    }
    void sendAfterTouch(uint8_t velocity) {
        sendMidiMessage(MidiType::AFTERTOUCH_CHANNEL, MIDI_CHANNEL, velocity, 0);
    }
    void sendPitchBend(uint16_t pitch) {
        uint16_t temp = map(pitch, 0, 1023, 0, EXTENDED_CONTROL_CHANGE_MAX);
        sendMidiMessage(MidiType::PITCH_BEND, MIDI_CHANNEL, (temp >> 7), (temp & 0xFF));
    }
    void sendSongPosition(uint8_t position) {}
    void sendSongSelect(uint8_t song) {}
    void sendTuneRequest(void) {}
    void sendClock(void) {
        sendMidiMessage(MidiType::CLOCK, 0, 0, 0);
    }
    void sendStart(void) {}
    void sendContinue(void) {}
    void sendStop(void) {}
    void sendActiveSense(void) {}
    void sendReset(void) {}

    void init () {
        USB::init();
        // UART::init(); // eventual places for MIDI via UART initiation
    }

    void usb_midi_task (void) {
        if (USB::MIDI::available) {
            uint32_t buffer_length = USB::MIDI::buffer_size();
            if (buffer_length) {
                for (int i = 0; i < buffer_length; i++) {
                    uint8_t packet[4];
                    USB::MIDI::get(packet);
                    handleMidiMessage(packet);
                }
            }
        }


    }

    void midi_task () {
        // if (UART::MIDI::available) {
        //     uint8_t packet[4];
        //     UART::MIDI::get(packet);
        //     handleMidiMessage(packet);
        // }
    }

    void update () {
        USB::update();
        usb_midi_task();
        // midi_task();
    }
}