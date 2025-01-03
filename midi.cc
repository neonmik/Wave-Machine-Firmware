#include "midi.h"

namespace MIDI {

    // -----------------------------------------------------------------------------------------------
    //                                          Input
    // -----------------------------------------------------------------------------------------------

    void handleMidiMessage(MidiMessage& input) {
        switch (input.type) {
            // at the top to not waste time.
            case MidiType::InvalidType:         break; // handleInvalidType();

            // near the top to give priority for timing
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

            default:                            printf("MIDI Message type not found!");                     break;
                
        }
    }

    //  MIDI Callbacks
    void handleNoteOff(uint8_t note, uint8_t velocity) {
        NOTE_HANDLING::noteOff(note, velocity);
    }
    void handleNoteOn(uint8_t note, uint8_t velocity) {
        if (velocity == 0) NOTE_HANDLING::noteOff(note, MIDI_DEFAULT_NOTE_OFF_VEL);
        else NOTE_HANDLING::noteOn(note, velocity);
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
                CONTROLS::setKnob(CONTROLS::Page::MAIN, 0, temp);
                break;
            case 71: // Vector
                CONTROLS::setKnob(CONTROLS::Page::MAIN, 1, temp);
                break;
            case 72: // Release
                CONTROLS::setKnob(CONTROLS::Page::ADSR, 3, temp);
                break;
            case 73: // Attack
                CONTROLS::setKnob(CONTROLS::Page::ADSR, 0, temp);
                break;
            case 75: // Decay
                CONTROLS::setKnob(CONTROLS::Page::ADSR, 1, temp);
                break;
            case 64: // Sustain pedal
                NOTE_HANDLING::setSustainPedal(temp);
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
        // easy way to map 14 bit range to the needed internal 10 bit.
        uint16_t temp = pitch >> 4;
        CONTROLS::setKnob(0, 3, temp);
    }
    void handleSysEx(MidiMessage message) {
        printf("SYSEX: \n");
        printMidiMessage(message);
    }
    void handleSongPosition(uint8_t position_msb, uint8_t position_lsb) {
        uint16_t temp = position_lsb << 7 | position_msb;
        printf("SONG POSITION POINTER: %d\n", temp);
        CLOCK::midiClockPosition(temp);
    }
    void handleSongSelect(uint8_t song) {}
    void handleTuneRequest(void) {}
    void handleClock(void) {
        CLOCK::midiClockTick();

    }
    void handleStart(void) {
        CLOCK::startMidiClock();
    }
    void handleStop(void) {
        CLOCK::stopMidiClock();
    }
    void handleContinue(void) {
        
    }
    void handleActiveSense(void) {}
    void handleReset(void) {}
    void handleInvalidType (MidiMessage message) {
        printf("Invlaid Midi Type made it through!\n");
        printMidiMessage(message);
    }
    

    // -----------------------------------------------------------------------------------------------
    //                                          Output
    // -----------------------------------------------------------------------------------------------
    void sendMidiMessage (uint8_t *msg, uint8_t length) {
        UART::MIDI::send(msg, length);
        USB::MIDI::send(msg, length);
    }

    void sendNoteOff(uint8_t note, uint8_t velocity) {
        uint8_t msg[3] = {(uint8_t(MidiType::NoteOff) | MIDI_CHANNEL), note, velocity};

        sendMidiMessage(msg, 3);
    }
    void sendNoteOn(uint8_t note, uint8_t velocity) {
        uint8_t msg[3] = {(uint8_t(MidiType::NoteOn) | MIDI_CHANNEL), note, velocity};

        sendMidiMessage(msg, 3);
    }
    void sendVelocityChange(uint8_t note, uint8_t velocity) {
        uint8_t msg[3] = {(uint8_t(MidiType::AfterTouchPoly) | MIDI_CHANNEL), note, velocity};
        
        sendMidiMessage(msg, 3);
    }
    void sendControlChange(uint8_t controller, uint8_t value){
        uint8_t msg[3] = {(uint8_t(MidiType::ControlChange) | MIDI_CHANNEL), controller, value};
        
        sendMidiMessage(msg, 3);
    }
    void sendProgramChange(uint8_t program) {
        uint8_t msg[2] = {(uint8_t(MidiType::ProgramChange) | MIDI_CHANNEL), program};
        
        sendMidiMessage(msg, 2);
    }
    void sendAfterTouch(uint8_t velocity) {
        uint8_t msg[2] = {(uint8_t(MidiType::AfterTouchChannel) | MIDI_CHANNEL), velocity};
        
        sendMidiMessage(msg, 2);
    }
    void sendPitchBend(uint16_t pitch) {
        // uint16_t temp = map(pitch, 0, 1023, 0, EXTENDED_CONTROL_CHANGE_MAX);
        uint16_t temp = pitch << 4;

        uint8_t msg[3] = {(uint8_t(MidiType::PitchBend) | MIDI_CHANNEL), uint8_t(temp >> 7), uint8_t(temp & 0xFF)};
        
        sendMidiMessage(msg, 3);
    }
    void sendSysEx(size_t length, const uint8_t* data) {
        // Create a MIDI message for the SysEx data
        uint8_t midiMessage[length+2];

        // Add SysEx start byte
        midiMessage[0] = 0xF0; // SysEx start

        // Add data bytes
        for (size_t i = 0; i < length; ++i) {
            midiMessage[i+1] = data[i];
        }

        // Add SysEx end byte
        midiMessage[length+1] = 0xF7; // SysEx end

        // Send the MIDI message
        sendMidiMessage(midiMessage, length);
    }
    void sendSongPosition(uint8_t position) {}
    void sendSongSelect(uint8_t song) {}
    void sendTuneRequest(void) {}
    void sendClock(void) {
        uint8_t msg[1] = {uint8_t(MidiType::Clock)};

        sendMidiMessage(msg, 1);
    }
    void sendStart(void) {}
    void sendContinue(void) {}
    void sendStop(void) {}
    void sendActiveSense(void) {}
    void sendReset(void) {}

    void toggleClockFlag (void) {
        // if (!sendClockFlag) sendClockFlag = true;
        // else DEBUG::warning("MIDI clock output not updating fast enough");
    }
    void checkClockFlag (void) {
        if (!sendClockFlag) return;
        
        MIDI::sendClock();
        sendClockFlag = false;
    }

    void init (void) {
        UART::init();
        USB::init();
    }

    void usb_midi_task (void) {
        int buffer_size;
        while (buffer_size = USB::MIDI::available()) {
            uint8_t packet[buffer_size];

            USB::MIDI::get(buffer_size, packet);

            inputMessageUSB.length =    buffer_size;
            inputMessageUSB.type =      getTypeFromStatusByte(packet[0]);

            if (inputMessageUSB.type != MidiType::SystemExclusive) {
                inputMessageUSB.channel =   getChannelFromStatusByte(packet[0]);
                inputMessageUSB.data1 =     packet[1];
                inputMessageUSB.data2 =     packet[2];
            } else {
                for (int i = 0; i < buffer_size; i++) {
                    
                    inputMessageUSB.dataSysex[i] = packet[i+1];
                }
            }
            
            printf("USB:        ");
            printMidiIn();
            printMidiMessage(inputMessageUSB);
            
            handleNullVelocity(inputMessageUSB);

            const bool channelMatch = inputFilter(inputMessageUSB);
            if (channelMatch)
                handleMidiMessage(inputMessageUSB);
            
        }
    }

    void midi_task () {
        if (!parse())
            return;

        // printf("UART:       ");
        // printMidiIn();
        // printMidiMessage(inputMessageUART);

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

    void update () {
        MIDI::checkClockFlag();

        if (MIDI_CHANNEL >= MIDI_CHANNEL_OFF)
            return;

        midi_task(); // MIDI task goes first cause it's slower... keeps the timing more aligned.

        USB::update();
        usb_midi_task();
    }

    void print (uint8_t *packet) {
        printf("MIDI IN: %02X", packet[0]);
        printf(" %02X", packet[1]);
        printf(" %02X", packet[2]);
        printf(" %02X\n", packet[3]);
    }
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
                printf(" %d", message.dataSysex[i]);
                if ((i + 1) % 8 == 0) {
                    printf("\n");
                }
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