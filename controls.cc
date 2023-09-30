#include "controls.h"

namespace CONTROLS {
    CONTROL Control;
    PRESET Preset[MAX_PRESETS];

    void init () {
        EEPROM::init();

        currentPreset = DEFAULT_PRESET;
        
        // factoryRestore();
        
        loadPreset(currentPreset);
    }

    void setPreset (uint8_t preset) {
        currentPreset = preset;

        loadPreset(currentPreset);

        PAGINATION::refresh();
    }
    
    uint8_t getPreset () {
        return currentPreset;
    }

    void savePreset (uint8_t preset) {

        Preset[preset].Wave.shape = Control.getKnob(Page::MAIN, 0);
        Preset[preset].Wave.vector = Control.getKnob(Page::MAIN, 1);
        Preset[preset].Wave.octave = Control.getKnob(Page::MAIN, 2);
        Preset[preset].Wave.pitch = Control.getKnob(Page::MAIN, 3);

            Preset[preset].Envelope.attack = Control.getKnob(Page::ADSR, 0);
            Preset[preset].Envelope.decay = Control.getKnob(Page::ADSR, 1);
            Preset[preset].Envelope.sustain = Control.getKnob(Page::ADSR, 2);
            Preset[preset].Envelope.release = Control.getKnob(Page::ADSR, 3);

        Preset[preset].Filter.state = true;

        Preset[preset].Filter.cutoff = Control.getKnob(Page::FILT, 0);
        Preset[preset].Filter.resonance = Control.getKnob(Page::FILT, 1);
        Preset[preset].Filter.punch = Control.getKnob(Page::FILT, 2);
        Preset[preset].Filter.type = Control.getKnob(Page::FILT, 3);

            Preset[preset].Filter.attack = Control.getKnob(Page::fENV, 0);
            Preset[preset].Filter.decay = Control.getKnob(Page::fENV, 1);
            Preset[preset].Filter.sustain = Control.getKnob(Page::fENV, 2);
            Preset[preset].Filter.release = Control.getKnob(Page::fENV, 3);

        Preset[preset].Modulation.state = Control.getButton(Page::LFO);

        Preset[preset].Modulation.matrix = Control.getKnob(Page::LFO, 0);
        Preset[preset].Modulation.rate = Control.getKnob(Page::LFO, 1);
        Preset[preset].Modulation.depth = Control.getKnob(Page::LFO, 2);
        Preset[preset].Modulation.wave = Control.getKnob(Page::LFO, 3);

            Preset[preset].Effects.gain = Control.getKnob(Page::SHFT, 3);

        Preset[preset].Arpeggiator.state = Control.getButton(Page::ARP);

        Preset[preset].Arpeggiator.gate = Control.getKnob(Page::ARP, 0);
        Preset[preset].Arpeggiator.divisions = Control.getKnob(Page::ARP, 1);
        Preset[preset].Arpeggiator.range = Control.getKnob(Page::ARP, 2);
        Preset[preset].Arpeggiator.direction = Control.getKnob(Page::ARP, 3);

            Preset[preset].Arpeggiator.rest = Control.getKnob(Page::sARP, 0);
            Preset[preset].Arpeggiator.bpm = Control.getKnob(Page::sARP, 1);
            Preset[preset].Arpeggiator.fMode = Control.getKnob(Page::sARP, 2);
            Preset[preset].Arpeggiator.octMode = Control.getKnob(Page::sARP, 3);

        EEPROM::savePreset(preset, Preset[preset]);
    }
    void loadPreset (uint8_t preset) {
        PRESET temp;
        
        EEPROM::loadPreset(preset, Preset[preset]);

        Control.setKnob(Page::MAIN, 0, Preset[preset].Wave.shape);
        Control.setKnob(Page::MAIN, 1, Preset[preset].Wave.vector);
        Control.setKnob(Page::MAIN, 2, Preset[preset].Wave.octave);
        Control.setKnob(Page::MAIN, 3, Preset[preset].Wave.pitch);

            Control.setKnob(Page::ADSR, 0, Preset[preset].Envelope.attack);
            Control.setKnob(Page::ADSR, 1, Preset[preset].Envelope.decay);
            Control.setKnob(Page::ADSR, 2, Preset[preset].Envelope.sustain);
            Control.setKnob(Page::ADSR, 3, Preset[preset].Envelope.release);

        // Control.set_filter(Preset[preset].Filter.state);

        Control.setKnob(Page::FILT, 0, Preset[preset].Filter.cutoff);
        Control.setKnob(Page::FILT, 1, Preset[preset].Filter.resonance);
        Control.setKnob(Page::FILT, 2, Preset[preset].Filter.punch);
        Control.setKnob(Page::FILT, 3, Preset[preset].Filter.type);

            Control.setKnob(Page::fENV, 0, Preset[preset].Filter.attack);
            Control.setKnob(Page::fENV, 1, Preset[preset].Filter.decay);
            Control.setKnob(Page::fENV, 2, Preset[preset].Filter.sustain);
            Control.setKnob(Page::fENV, 3, Preset[preset].Filter.release);


        // Control.set_lfo(Preset[preset].Modulation.state);
        Control.setButton(Page::LFO, Preset[preset].Modulation.state);

        Control.setKnob(Page::LFO, 0, Preset[preset].Modulation.matrix);
        Control.setKnob(Page::LFO, 1, Preset[preset].Modulation.rate);
        Control.setKnob(Page::LFO, 2, Preset[preset].Modulation.depth);
        Control.setKnob(Page::LFO, 3, Preset[preset].Modulation.wave);

            Control.setKnob(Page::SHFT, 0, 0);
            Control.setKnob(Page::SHFT, 1, 0);
            Control.setKnob(Page::SHFT, 2, Preset[preset].Effects.gain);
            Control.setKnob(Page::SHFT, 3, 0);


        // Control.set_arp(Preset[preset].Arpeggiator.state);
        Control.setButton(Page::ARP, Preset[preset].Arpeggiator.state);

        Control.setKnob(Page::ARP, 0, Preset[preset].Arpeggiator.gate);
        Control.setKnob(Page::ARP, 1, Preset[preset].Arpeggiator.divisions);
        Control.setKnob(Page::ARP, 2, Preset[preset].Arpeggiator.range);
        Control.setKnob(Page::ARP, 3, Preset[preset].Arpeggiator.direction);

            Control.setKnob(Page::sARP, 0, Preset[preset].Arpeggiator.rest);
            Control.setKnob(Page::sARP, 1, Preset[preset].Arpeggiator.bpm);
            Control.setKnob(Page::sARP, 2, Preset[preset].Arpeggiator.fMode);
            Control.setKnob(Page::sARP, 3, Preset[preset].Arpeggiator.octMode);

        Control.updateAll();
        // needsUpdating = true;
    }
    void exportPresets(void) {
        PRESET export_buffer[MAX_PRESETS];

        for (int i = 0; i < MAX_PRESETS; i++) {
            EEPROM::loadPreset(i, export_buffer[i]);
        }

        // send export_buffer somewhere?    - maybe save as a file that can be accessed in USB?
        //                                  - send over MIDI
    }

    // Function for restoring the Factory Presets from the EEPROM to the main preset storage area.
    void factoryRestore (void) {

        printf("\nFactory Restore in progress!\n");

        for (int i = 0; i < MAX_PRESETS; i++) {
            EEPROM::restoreFactoryPreset(i);
        }
        
        printf("Factory Settings restored!\n\n");
        
        setPreset(currentPreset); // not sure if this needs to be here? just needs to make sure it updates the settings right after factory restore, seems like the safest way to do it
    }

    // Function for writing current Presets to the Factory Preset storage area
    void updateFactoryPresets (void) {

        printf("Storing currrent presets to Factory Preset slots!\n");

        for (int i = 0; i < MAX_PRESETS; i++) {
            EEPROM::writeFactoryPreset(i);
        }

        printf("All Presets backed up!\n\n");


    }

    // Save current Preset
    void save () {
        savePreset(currentPreset);
    }

    void setPage (uint8_t page) {
        needsUpdating = true;
        currentPage = page;

        PAGINATION::refresh();
    }
    uint8_t getPage (void) {
        return currentPage;
    }

    void setKnob (uint8_t page, uint8_t control, uint16_t input) {
        needsUpdating = true;

        if (shift) page += 4; // This is here to make sure it doesn't interfere with preset loading and saving.

        Control.setKnob(page, control, input);
    }
    uint16_t getKnob (uint8_t page, uint8_t control) {
        if (shift) page += 4; // This is here to make sure it doesn't interfere with preset loading and saving.

        return Control.getKnob(page, control);
    }

    void setButton (uint8_t page, bool state) {
        if (shift) page += 4; // This is here to make sure it doesn't interfere with preset loading and saving.

        Control.setButton(page, state);
    }
    void toggleButton (uint8_t page) {
        if (shift) page += 4; // This is here to make sure it doesn't interfere with preset loading and saving.

        Control.toggleButton(page);
    }
    bool getButton (uint8_t page) {
        if (shift) page += 4; // This is here to make sure it doesn't interfere with preset loading and saving.
        
        return Control.getButton(page);
    }
       
    void toggleLFO () {
        needsUpdating = true;
        Control.toggleButton(Page::LFO);
    }
    bool getLFO () {
        return Control.getButton(Page::LFO);
    }

    void toggleArp () {
        needsUpdating = true;
        Control.toggleButton(Page::ARP);
    }
    bool getArp () {
        return Control.getButton(Page::ARP);
    }

    void toggleShift (void) {
        needsUpdating = true;
        shift = !shift;

        PAGINATION::refresh();
    }
    bool getShift (void) {
        return shift;
    }
    
    void update () {
        if (needsUpdating) {
            uint8_t temp = currentPage;
            if (shift) temp += 4; // This is here to make sure it doesn't interfere with preset loading and saving.
            
            Control.updateAll();
            
            needsUpdating = false;
        }
    }
    
}                                                                             