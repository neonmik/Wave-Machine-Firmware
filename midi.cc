#include "midi.h"

namespace MIDI {

    // -----------------------------------------------------------------------------------------------
    //                                          Input
    // -----------------------------------------------------------------------------------------------

    void handleMidiMessage(MidiMessage& input) {
        switch (input.type) {
            // at the top to give priority for timing
            case MidiType::Clock:               handleClock();                                              break;
            
            case MidiType::Start:               handleStart();                                              break;

            case MidiType::Continue:            handleContinue();                                           break;

            case MidiType::Stop:                handleStop();                                               break;

            case MidiType::NoteOff:             handleNoteOff(input.data1, input.data2);                    break;

            case MidiType::NoteOn:              handleNoteOn(input.data1, input.data2);                     break;

            case MidiType::AfterTouchPoly:      handleVelocityChange(input.data1, input.data2);             break;

            case MidiType::ControlChange:       handleControlChange(input.data1, input.data2);              break;

            case MidiType::ProgramChange:       handleProgramChange(input.data1);                           break;

            case MidiType::AfterTouchChannel:   handleAfterTouch(input.data1);                              break;

            case MidiType::PitchBend:           handlePitchBend(((input.data2 << 7) | (input.data1)));      break;

            case MidiType::SystemExclusive:     handleSysEx(input);                                         break;

            case MidiType::SongPosition:        handleSongPosition(input.data1, input.data2);               break;

            case MidiType::SongSelect:          handleSongSelect(input.data1);                              break;

            case MidiType::TuneRequest:         handleTuneRequest();                                        break;

            case MidiType::ActiveSensing:       handleActiveSense();                                        break;

            case MidiType::SystemReset:         handleReset();                                              break;

            case MidiType::InvalidType:         handleInvalidType(input);                                   break;

            default:                            printf("UART MIDI Message type not found!");                break;
                
        }
    }

    // old midi handling code.
    // void handleMidiMessage(uint8_t msg[3]) {
    //     uint8_t midiType, channel, data1, data2;
    //     if (msg[0] >= 0xF0) { // check if SYS EX, as they use the upper half of the status byte
    //         midiType = msg[0]; // Extract the MIDI type
    //         // channel = 0; // not used here.
    //         data1 = msg[1]; // Extract the first data byte
    //         data2 = msg[2]; // Extract the second data byte
    //     } else { // everything else uses it for channel data
    //         midiType = msg[0] & 0xF0; // Extract the MIDI type
    //         channel = msg[0] & 0x0F; // Extract the channel from the status byte
    //         if (channel != MIDI_CHANNEL) return; // if the channel doesn't match, not point checking the rest... Add in OMNI mode option here.
    //         data1 = msg[1]; // Extract the first data byte
    //         data2 = msg[2]; // Extract the second data byte
    //     }
    // 
    //     switch (midiType) {
    //         case MidiType::Clock: // at the top to give priority for timing
    //             handleClock();
    //             break;
    //  
    //         case MidiType::Start:
    //             handleStart();
    //             break;
    // 
    //         case MidiType::Continue:
    //             handleContinue();
    //             break;
    // 
    //         case MidiType::Stop:
    //             handleStop();
    //             break;
    // 
    //         case MidiType::NoteOff:
    //             handleNoteOff(channel, data1, data2);
    //             break;
    // 
    //         case MidiType::NoteOn:
    //             handleNoteOn(channel, data1, data2);
    //             break;
    // 
    //         case MidiType::AfterTouchPoly:
    //             handleVelocityChange(channel, data1, data2);
    //             break;
    // 
    //         case MidiType::ControlChange:
    //             handleControlChange(channel, data1, data2);
    //             break;
    // 
    //         case MidiType::ProgramChange:
    //             handleProgramChange(channel, data1);
    //             break;
    // 
    //         case MidiType::AfterTouchChannel:
    //             handleAfterTouch(channel, data1);
    //             break;
    // 
    //         case MidiType::PitchBend:
    //             handlePitchBend(channel, ((data2 << 7) | (data1)));
    //             break;
    // 
    //         case MidiType::SystemExclusive:
    //             break;
    // 
    //         case MidiType::SongPosition: 
    //             handleSongPosition(data1, data2);
    //             break;
    // 
    //         case MidiType::SongSelect:
    //             handleSongSelect(data1);
    //             break;
    // 
    //         case MidiType::TuneRequest:
    //             handleTuneRequest();
    //             break;
    // 
    //         case MidiType::ActiveSensing:
    //             handleActiveSense();
    //             break;
    // 
    //         case MidiType::SystemReset:
    //             handleReset();
    //             break;
    // 
    //         case MidiType::InvalidType:
    //             // This is called if the midi message is empty...
    //             // shouldn't happen very often, if at all. 
    //             printf("Invalid MIDI Message type!      ");
    //             print(msg);
    //             break;
    // 
    //         default:
    //             // should never get here
    //             printf("MIDI Message type not found!    ");
    //             print(msg);
    //             break;
    //     }
    // }
    
    
    //  MIDI Callbacks
    void handleNoteOff(uint8_t note, uint8_t velocity) {
        NOTE_HANDLING::note_off(note, velocity);
    }
    void handleNoteOn(uint8_t note, uint8_t velocity) {
        if (velocity == 0) NOTE_HANDLING::note_off(note, MIDI_DEFAULT_NOTE_OFF_VEL);
        else NOTE_HANDLING::note_on(note, velocity);
    }
    void handleVelocityChange(uint8_t note, uint8_t velocity) {
        return;
    }
    void handleControlChange(uint8_t controller, uint8_t value) {

        // add 14-bit midi handling code here too...
        // if ((CC < 64) && ((CC + 32)>0)) 14bit_mode = true;
        // if (14bit_mode) uint16_t temp = map(value, CONTROL_CHANGE_MIN, EXTENDED_CONTROL_CHANGE_MAX, KNOB_MIN, KNOB_MAX);
        // else uint16_t temp = map(value, CONTROL_CHANGE_MIN, CONTROL_CHANGE_MAX, KNOB_MIN, KNOB_MAX);

        uint16_t temp = value << 3; // bit shifts the 7 bit input value to the 10 bits needed internally
                                    // could add a LUT with interpolation here to improve the resolution.
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
    void handleProgramChange(uint8_t program) {
    }
    void handleAfterTouch(uint8_t velocity) {
    }
    void handlePitchBend(uint16_t pitch) {
        uint16_t temp = pitch >> 4; // easy way to map 14 bit range to the needed internal 10 bit.
        CONTROLS::set_value(0, 3, temp);
    }
    void handleSysEx(MidiMessage message) {
        printf("SysEx Message has nowhere to go...\n");
        printMidiMessage(message);
    }
    void handleSongPosition(uint8_t position_msb, uint8_t position_lsb) {}
    void handleSongSelect(uint8_t song) {}
    void handleTuneRequest(void) {}
    void handleClock(void) {
        CLOCK::midi_tick();
    }
    void handleStart(void) {
        CLOCK::start_midi_clock();
    }
    void handleStop(void) {
        CLOCK::stop_midi_clock();
    }
    void handleContinue(void) {}
    void handleActiveSense(void) {}
    void handleReset(void) {}
    void handleInvalidType (MidiMessage message) {
        printf("Invlaid Midi Type made it through!\n");
        printMidiMessage(message);
    }
    

    // -----------------------------------------------------------------------------------------------
    //                                          Output
    // -----------------------------------------------------------------------------------------------
    void sendMidiMessage (uint8_t type, uint8_t channel, uint8_t data1, uint8_t data2) {
        uint8_t status = (type | channel);
        uint8_t msg[3] = { status, data1, data2 };
        UART::MIDI::send(msg);
        USB::MIDI::send(msg);
    }

    void sendNoteOff(uint8_t note, uint8_t velocity) {
        sendMidiMessage(MidiType::NoteOff, MIDI_CHANNEL, note, velocity);
    }
    void sendNoteOn(uint8_t note, uint8_t velocity) {
        sendMidiMessage(MidiType::NoteOn, MIDI_CHANNEL, note, velocity);
    }
    void sendVelocityChange(uint8_t note, uint8_t velocity) {
        sendMidiMessage(MidiType::AfterTouchPoly, MIDI_CHANNEL, note, velocity);
    }
    void sendControlChange(uint8_t controller, uint8_t value){
        sendMidiMessage(MidiType::ControlChange, MIDI_CHANNEL, controller, value);
    }
    void sendProgramChange(uint8_t program) {
        sendMidiMessage(MidiType::ProgramChange, MIDI_CHANNEL, program, 0);
    }
    void sendAfterTouch(uint8_t velocity) {
        sendMidiMessage(MidiType::AfterTouchChannel, MIDI_CHANNEL, velocity, 0);
    }
    void sendPitchBend(uint16_t pitch) {
        uint16_t temp = map(pitch, 0, 1023, 0, EXTENDED_CONTROL_CHANGE_MAX);
        sendMidiMessage(MidiType::PitchBend, MIDI_CHANNEL, (temp >> 7), (temp & 0xFF));
    }
    void sendSongPosition(uint8_t position) {}
    void sendSongSelect(uint8_t song) {}
    void sendTuneRequest(void) {}
    void sendClock(void) {
        sendMidiMessage(MidiType::Clock, 0, 0, 0);
    }
    void sendStart(void) {}
    void sendContinue(void) {}
    void sendStop(void) {}
    void sendActiveSense(void) {}
    void sendReset(void) {}

    void Init () {
        UART::Init();
        USB::Init();
    }

    void usb_midi_task (void) {
        int buffer_size;
        while (buffer_size = USB::MIDI::available()) {
            uint8_t packet[3];

            packet[0] = 0;
            packet[1] = 0;
            packet[2] = 0;

            USB::MIDI::get(packet);

            inputMessageUSB.type =      getTypeFromStatusByte(packet[0]);
            inputMessageUSB.channel =   getChannelFromStatusByte(packet[0]);
            inputMessageUSB.data1 =     packet[1];
            inputMessageUSB.data2 =     packet[2];
            
            printf("USB:    ");
            printMidiIn();
            printMidiMessage(inputMessageUSB);
            
            handleNullVelocity(inputMessageUSB);

            // handleMidiMessage(packet); // old midi note handling.
            const bool channelMatch = inputFilter(inputMessageUART);
            if (channelMatch)
                handleMidiMessage(inputMessageUSB);
            
        }
    }

    void midi_task () {
        if (!parse())
            return;

        printf("UART:   ");
        printMidiIn();
        printMidiMessage(inputMessageUART);

        handleNullVelocity(inputMessageUART);

        const bool channelMatch = inputFilter(inputMessageUART);
        if (channelMatch)
            handleMidiMessage(inputMessageUART);
    }

    MidiType getTypeFromStatusByte(uint8_t inStatus) {
        if ((inStatus  < 0x80) ||
            (inStatus == Undefined_F4) ||
            (inStatus == Undefined_F5) ||
            (inStatus == Undefined_FD))
            return InvalidType; // Data bytes and undefined.

        if (inStatus < 0xf0)
            // Channel message, remove channel nibble.
            return MidiType(inStatus & 0xf0);

        return MidiType(inStatus);
    }

    uint8_t getChannelFromStatusByte(uint8_t inStatus) {
        return ((inStatus & 0x0f) + 1);
    }

    bool isChannelMessage(MidiType inType) {
        return (inType == NoteOff           ||
                inType == NoteOn            ||
                inType == ControlChange     ||
                inType == AfterTouchPoly    ||
                inType == AfterTouchChannel ||
                inType == PitchBend         ||
                inType == ProgramChange);
    }
    
    void handleNullVelocity(MidiMessage message) {
        if (NullVelocityNoteOnAsNoteOff &&
            message.type == NoteOn && message.data2 == 0)
        {
            message.type = NoteOff;
        }
    }
    bool inputFilter(MidiMessage message) {
        // This method handles recognition of channel
        // (to know if the message is destinated to the Arduino)

        // First, check if the received message is Channel
        if (message.type >= NoteOff && message.type <= PitchBend) {
            // Then we need to know if we listen to it
            if ((message.channel == MIDI_CHANNEL) ||
                (MIDI_CHANNEL == MIDI_CHANNEL_OMNI)) {
                    return true;
            } else {
                // We don't listen to this channel
                return false;
            }
        } else {
            // System messages are always received
            return true;
        }
    }

    void resetInput (void) {
        pendingMessageIndex = 0;
        pendingMessageExpectedLength = 0;
        runningStatus = InvalidType;
    }

    bool parse (void) {
        if (UART::MIDI::available() == 0) 
            return false; // No data available.


        // Parsing algorithm:
        // Get a byte from the serial buffer.
        // If there is no pending message to be recomposed, start a new one.
        //  - Find type and channel (if pertinent)
        //  - Look for other bytes in buffer, call parser recursively,
        //    until the message is assembled or the buffer is empty.
        // Else, add the extracted byte to the pending message, and check validity.
        // When the message is done, store it.

        const uint8_t extracted = UART::MIDI::get();

        if (extracted == Undefined_FD) 
            return (Use1ByteParsing) ? false : parse();

        if (pendingMessageIndex == 0) {

            pendingMessage[0] = extracted;

            if (isChannelMessage(getTypeFromStatusByte(runningStatus)))
            {
                // Only these types allow Running Status

                // If the status byte is not received, prepend it
                // to the pending message
                if (extracted < 0x80)
                {
                    pendingMessage[0]   = runningStatus;
                    pendingMessage[1]   = extracted;
                    pendingMessageIndex = 1;
                }
                // Else: well, we received another status byte,
                // so the running status does not apply here.
                // It will be updated upon completion of this message.
            }

            const MidiType pendingType = getTypeFromStatusByte(extracted);

            switch (pendingType) {
                // 1 byte messages
                case MidiType::Start:
                case MidiType::Continue:
                case MidiType::Stop:
                case MidiType::Clock:
                case MidiType::Tick:
                case MidiType::ActiveSensing:
                case MidiType::SystemReset:
                case MidiType::TuneRequest:
                    // Handle the message type directly here.
                    inputMessageUART.type    = pendingType;
                    inputMessageUART.channel = 0;
                    inputMessageUART.data1   = 0;
                    inputMessageUART.data2   = 0;
                    inputMessageUART.valid   = true;

                    // Do not reset all input attributes, Running Status must remain unchanged.
                    // We still need to reset these
                    pendingMessageIndex = 0;
                    pendingMessageExpectedLength = 0;

                    return true;
                    break;

                // 2 bytes messages
                case MidiType::ProgramChange:
                case MidiType::AfterTouchChannel:
                case MidiType::TimeCodeQuarterFrame:
                case MidiType::SongSelect:
                    pendingMessageExpectedLength = 2;
                    break;

                // 3 bytes messages
                case MidiType::NoteOn:
                case MidiType::NoteOff:
                case MidiType::ControlChange:
                case MidiType::PitchBend:
                case MidiType::AfterTouchPoly:
                case MidiType::SongPosition:
                    pendingMessageExpectedLength = 3;
                    break;

                case MidiType::SystemExclusiveStart:
                case MidiType::SystemExclusiveEnd:
                    // The message can be any length
                    // between 3 and MidiMessage::sSysExMaxSize bytes
                    pendingMessageExpectedLength = SYSEX_MAX_SIZE;
                    runningStatus = InvalidType;
                    inputMessageUART.dataSysex[0] = pendingType;
                    break;

                case MidiType::InvalidType:
                default:
                    // This is obviously wrong. Let's get the hell out'a here.
                    printf("MIDI parsing error!");
                    return false;
                    break;
            }

            if (pendingMessageIndex >= (pendingMessageExpectedLength - 1))
            {
                // Reception complete
                inputMessageUART.type    = pendingType;
                inputMessageUART.channel = getChannelFromStatusByte(pendingMessage[0]);
                inputMessageUART.data1   = pendingMessage[1];
                inputMessageUART.data2   = 0; // Completed new message has 1 data byte
                inputMessageUART.length  = 1;

                pendingMessageIndex = 0;
                pendingMessageExpectedLength = 0;
                inputMessageUART.valid = true;

                return true;
            }
            else
            {
                // Waiting for more data
                pendingMessageIndex++;
            }

            return (Use1ByteParsing) ? false : parse(); // if the setting for 1 byte at a time is active, return it, if not, loop again.
        } else {
            // First, test if this is a status byte
            if (extracted >= 0x80) {
                // Reception of status bytes in the middle of an uncompleted message
                // are allowed only for interleaved Real Time message or EOX
                switch (extracted)
                {
                    case MidiType::Clock:
                    case MidiType::Start:
                    case MidiType::Tick:
                    case MidiType::Continue:
                    case MidiType::Stop:
                    case MidiType::ActiveSensing:
                    case MidiType::SystemReset:

                        // Here we will have to extract the one-byte message,
                        // pass it to the structure for being read outside
                        // the MIDI class, and recompose the message it was
                        // interleaved into. Oh, and without killing the running status..
                        // This is done by leaving the pending message as is,
                        // it will be completed on next calls.

                        inputMessageUART.type    = (MidiType)extracted;
                        inputMessageUART.data1   = 0;
                        inputMessageUART.data2   = 0;
                        inputMessageUART.channel = 0;
                        inputMessageUART.length  = 1;
                        inputMessageUART.valid   = true;

                        return true;

                        // Exclusive
                    case MidiType::SystemExclusiveStart:
                    case MidiType::SystemExclusiveEnd:
                        if ((inputMessageUART.dataSysex[0] == SystemExclusiveStart) ||
                            (inputMessageUART.dataSysex[0] == SystemExclusiveEnd)) {
                            // Store the last byte (EOX)
                            inputMessageUART.dataSysex[pendingMessageIndex++] = extracted;
                            inputMessageUART.type = SystemExclusive;

                            // Get length
                            inputMessageUART.data1   = pendingMessageIndex & 0xff; // LSB
                            inputMessageUART.data2   = uint8_t(pendingMessageIndex >> 8);   // MSB
                            inputMessageUART.channel = 0;
                            inputMessageUART.length  = pendingMessageIndex;
                            inputMessageUART.valid   = true;

                            resetInput();

                            return true;
                        }
                        else
                        {
                            // Well well well.. error.
                            printf("error!?");
                            return false;
                        }
                    // LCOV_EXCL_START - Coverage blind spot
                    default:
                        break;
                    // LCOV_EXCL_STOP
                }
            }
            
            // Add extracted data byte to pending message
            if ((pendingMessage[0] == SystemExclusiveStart) ||  
                (pendingMessage[0] == SystemExclusiveEnd)) {
                inputMessageUART.dataSysex[pendingMessageIndex] = extracted;
            } else {
                pendingMessage[pendingMessageIndex] = extracted;
            }
            // Now we are going to check if we have reached the end of the message
            if (pendingMessageIndex >= (pendingMessageExpectedLength - 1)) {
                // SysEx larger than the allocated buffer size,
                // Split SysEx like so:
                //   first:  0xF0 .... 0xF0
                //   midlle: 0xF7 .... 0xF0
                //   last:   0xF7 .... 0xF7
                if ((pendingMessage[0] == SystemExclusiveStart)
                ||  (pendingMessage[0] == SystemExclusiveEnd))
                {
                    uint8_t lastByte = inputMessageUART.dataSysex[SYSEX_MAX_SIZE - 1];
                    inputMessageUART.dataSysex[SYSEX_MAX_SIZE - 1] = uint8_t(SystemExclusiveStart);
                    inputMessageUART.type = SystemExclusive;

                    // Get length
                    inputMessageUART.data1   = SYSEX_MAX_SIZE & 0xff; // LSB
                    inputMessageUART.data2   = uint8_t(SYSEX_MAX_SIZE >> 8); // MSB
                    inputMessageUART.channel = 0;
                    inputMessageUART.length  = SYSEX_MAX_SIZE;
                    inputMessageUART.valid   = true;

                    // No need to check against the inputChannel,
                    // SysEx ignores input channel
                    handleMidiMessage(inputMessageUART);

                    inputMessageUART.dataSysex[0] = SystemExclusiveEnd;
                    inputMessageUART.dataSysex[1] = lastByte;

                    pendingMessageIndex = 2;

                    return false;
                }

                inputMessageUART.type = getTypeFromStatusByte(pendingMessage[0]);

                if (isChannelMessage(inputMessageUART.type))
                    inputMessageUART.channel = getChannelFromStatusByte(pendingMessage[0]);
                else
                    inputMessageUART.channel = 0;

                inputMessageUART.data1 = pendingMessage[1];
                // Save data2 only if applicable
                inputMessageUART.data2 = pendingMessageExpectedLength == 3 ? pendingMessage[2] : 0;
                inputMessageUART.length = pendingMessageExpectedLength;

                // Reset local variables
                pendingMessageIndex = 0;
                pendingMessageExpectedLength = 0;

                inputMessageUART.valid = true;

                // Activate running status (if enabled for the received type)
                switch (inputMessageUART.type) {
                    case NoteOff:
                    case NoteOn:
                    case AfterTouchPoly:
                    case ControlChange:
                    case ProgramChange:
                    case AfterTouchChannel:
                    case PitchBend:
                        // Running status enabled: store it from received message
                        runningStatus = MidiType(pendingMessage[0]);
                        break;

                    default:
                        // No running status
                        runningStatus = InvalidType;
                        break;
                }
                return true;
            } else {
                // Then update the index of the pending message.
                pendingMessageIndex++;

                return (Use1ByteParsing) ? false : parse();
            }
        }
    }

    void Update () {
        if (MIDI_CHANNEL >= MIDI_CHANNEL_OFF)
            return;

        midi_task(); // MIDI task goes first cause it's slower... keeps the timing more aligned.

        USB::Update();
        usb_midi_task();
    }

    // void print (uint8_t *packet) {
    //     printf("MIDI IN: %02X", packet[0]);
    //     printf(" %02X", packet[1]);
    //     printf(" %02X", packet[2]);
    //     printf(" %02X\n", packet[3]);
    // }
    void printMidiIn (void) {
        printf("MIDI IN:    ");
    }
    void printMidiOut (void) {
        printf("MIDI OUT:   ");
    }
    void printMidiType (MidiType type) {
        switch (type) {
            // at the top to give priority for timing
            case MidiType::Clock:               printf("Clock       ");     break;
            
            case MidiType::Start:               printf("Start       ");     break;

            case MidiType::Continue:            printf("Continue    ");     break;

            case MidiType::Stop:                printf("Stop        ");     break;

            case MidiType::NoteOff:             printf("NoteOff     ");     break;

            case MidiType::NoteOn:              printf("NoteOn      ");     break;

            case MidiType::AfterTouchPoly:      printf("AT Poly     ");     break;

            case MidiType::ControlChange:       printf("CC          ");     break;

            case MidiType::ProgramChange:       printf("PC          ");     break;

            case MidiType::AfterTouchChannel:   printf("AT Chan     ");     break;

            case MidiType::PitchBend:           printf("PitchBend   ");     break;

            case MidiType::SystemExclusive:     printf("SysEx       ");     break;

            case MidiType::SongPosition:        printf("Song Pos.   ");     break;

            case MidiType::SongSelect:          printf("Song Sel.   ");     break;

            case MidiType::TuneRequest:         printf("Tune        ");     break;

            case MidiType::ActiveSensing:       printf("ActiveSense ");     break;

            case MidiType::SystemReset:         printf("Sys Reset   ");     break;

            case MidiType::InvalidType:         printf("Invalid!    ");     break;

            default:                            printf("UART MIDI type not found!");     break;
        }    
    }
    void printMidiMessage (MidiMessage message) {
        if (message.type == MidiType::SystemExclusive) {
            printMidiType(message.type);
            printf(":");
            for (int i = 0; i < message.length; i++) {
                if (i & 0x8) printf("\n");
                printf(" %d", message.dataSysex[i]);
            }
            printf("\n");
        } else {
            printMidiType(message.type);
            printf(": Ch: %02d", message.channel);
            printf(" %03d", message.data1);
            printf(" %03d\n", message.data2);
        }
    }
}