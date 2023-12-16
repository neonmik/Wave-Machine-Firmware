#include "preset.h"

namespace PRESET {
    void init () {
        EEPROM::init();

        // EEPROM::eraseAll();

        // printSizes();

        // EEPROM::testIntegrity();
    }

    void save(uint8_t slot, SynthPreset &preset) {
        if (!EEPROM::checkSlot(slot)) {
            return;
        }

        printf("Saving Preset to slot %d\n", slot);
        printData(preset);

        uint16_t size = sizeof(preset);
        uint16_t address = (slot * PRESET_SIZE) | (PRESET_ADDRESS);

        // Create a buffer to hold the data - we use a preset size (3 pages/192 bytes) to ensure data is spaced properly
        // We init with 0 to make sure theres no data pulled/left over from elsewhere.
        uint8_t buffer[size] = {0};

        // Use memcpy to copy the data from preset to buffer, this also formats it as bytes for the EEPROM.
        memcpy(buffer, &preset, size);

        // Write the data in buffer to EEPROM
        EEPROM::write(address, buffer, size);
    }

    void load (uint8_t slot, SynthPreset &preset) {
        if (!EEPROM::checkSlot(slot)) {
            return;
        }
        
        uint16_t size = sizeof(preset);
        uint16_t address = (slot * PRESET_SIZE) | (PRESET_ADDRESS);

        // Create a buffer to hold the data - currently using sizeof to make sure we don't overflow the actual struct allocation.
        uint8_t buffer[size] = {0};
        
        // Read the data from the EEPROM address
        EEPROM::read(address, buffer, size);

        memcpy(&preset, buffer, size);

        // printf("Loading Preset from slot %d\n", slot);
        // printData(preset);
    }
    
   
    void factoryRestore (uint8_t slot) {
        if (!EEPROM::checkSlot(slot)) {
            return;
        }
        // load Factory Preset from EEPROM into buffer
        uint16_t factory_preset_address = (slot * PRESET_SIZE) | (FACTORY_PRESET_ADDRESS);
        uint16_t output_address = slot * PRESET_SIZE;
        
        transfer(factory_preset_address, output_address);
    }
    void factoryWrite (uint8_t slot) {
        if (!EEPROM::checkSlot(slot)) {
            return;
        }
        uint16_t preset_address = slot * PRESET_SIZE;
        uint16_t factory_preset_address = (slot * PRESET_SIZE) | (FACTORY_PRESET_ADDRESS);

        transfer(preset_address, factory_preset_address);
    }


    void printData (SynthPreset &preset) {
        printf("====================================================\n");
        printf("                    PRESET DATA                     \n");
        printf("====================================================\n\n");

        printf("Version:        %04d\n",      preset.version);
        printf("Length:         %04d\n\n",      preset.length);

        printf("----------------------------------------------------\n");
        printf("                      VOICE\n");
        printf("----------------------------------------------------\n\n");

        printf("Oscillator 1            |        Oscillator2\n");
        printf("Waveshape:      %04d    |        Waveshape:      %04d\n",      preset.Voice.Oscillator1.shape,      preset.Voice.Oscillator2.shape);
        printf("Vector:         %04d    |        Vector:         %04d\n",      preset.Voice.Oscillator1.vector,     preset.Voice.Oscillator2.vector);
        printf("-----------------------------------------------------\n");
        printf("Noise:          %04d    |        Detune:         %04d\n",      preset.Voice.noiseLevel,             preset.Voice.detune);
        printf("Octave:         %04d    |        Pitch:          %04d\n\n",        preset.Voice.octave,                 preset.Voice.pitchBend);

        printf("Attack:         %04d\n",      preset.Voice.Envelope.attack);
        printf("Decay:          %04d\n",      preset.Voice.Envelope.decay);
        printf("Sustain:        %04d\n",      preset.Voice.Envelope.sustain);
        printf("Release:        %04d\n\n",    preset.Voice.Envelope.release);
        

        printf("----------------------------------------------------\n");
        printf("                     FILTER\n");
        printf("----------------------------------------------------\n\n");

        printf("State:          ");
        if (preset.States.stateFLT == true) { printf("Enabled\n"); }
        else { printf("Disabled\n"); }

        printf("Cutoff:         %04d    |        Env. Direction: %04d\n",      preset.Filter.cutoff,    preset.Filter.direction);
        printf("Resonance:      %04d    |        Key Tracking:   %04d\n",      preset.Filter.resonance, preset.Filter.keyTracking);
        printf("Punch:          %04d    |        Env. Depth:     %04d\n",      preset.Filter.punch,     preset.Filter.envelopeDepth);
        printf("Type:           %04d    |        Env. Trigger:   %04d\n\n",    preset.Filter.type,      preset.Filter.triggerMode);

        printf("Attack:         %04d\n",      preset.Filter.Envelope.attack);
        printf("Decay:          %04d\n",      preset.Filter.Envelope.decay);
        printf("Sustain:        %04d\n",      preset.Filter.Envelope.sustain);
        printf("Release:        %04d\n\n",    preset.Filter.Envelope.release);


        printf("----------------------------------------------------\n");
        printf("                      LFO\n");
        printf("----------------------------------------------------\n\n");

        printf("State:          ");
        if (preset.States.stateLFO == true) { printf("Enabled\n"); }
        else { printf("Disabled\n"); }

        printf("Destination:    %04d\n",      preset.Modulation.matrix);
        printf("Rate:           %04d\n",      preset.Modulation.rate);
        printf("Depth:          %04d\n",      preset.Modulation.depth);
        printf("Shape:          %04d\n\n",    preset.Modulation.wave);

        printf("Attack:         %04d\n",      preset.Modulation.Envelope.attack);
        printf("Decay:          %04d\n",      preset.Modulation.Envelope.decay);
        printf("Sustain:        %04d\n",      preset.Modulation.Envelope.sustain);
        printf("Release:        %04d\n\n",    preset.Modulation.Envelope.release);
        

        printf("----------------------------------------------------\n");
        printf("                      ARP\n");
        printf("----------------------------------------------------\n\n");

        printf("State:          ");
        if (preset.States.stateARP == true) { printf("Enabled\n"); }
        else { printf("Disabled\n"); }

        printf("Gate:           %04d    |        BPM:            %04d\n",      preset.Arpeggiator.gate, preset.Arpeggiator.bpm);
        printf("Rate/Division:  %04d    |        Synced:         %04d\n",      preset.Arpeggiator.divisions, preset.Arpeggiator.synced);
        printf("Depth:          %04d    |        Octave Mode:    %04d\n",      preset.Arpeggiator.range, preset.Arpeggiator.octaveMode);
        printf("Shape:          %04d    |        Played Order:   %04d\n\n",     preset.Arpeggiator.direction, preset.Arpeggiator.playedOrder);



        printf("----------------------------------------------------\n");
        printf("                      FX\n");
        printf("----------------------------------------------------\n\n");

        printf("Gain:           %04d\n\n",      preset.Effects.gain);
    }
}