#include "controls.h"

namespace CONTROLS {
    CONTROL Control;
    PRESET Preset[MAX_PRESETS];

    void init () {
        PAGINATION::init();

        EEPROM::init();

        currentPreset = DEFAULT_PRESET;
        
        // factoryRestore();
        
        loadPreset(currentPreset);

        setPage(currentPage);
    }

    void setPreset (uint8_t preset) {
        currentPreset = preset;

        loadPreset(currentPreset);

        PAGINATION::refresh();
    }
    void changePreset (void) {
        ++currentPreset;
        if (currentPreset >= MAX_PRESETS) currentPreset = 0;

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

        Preset[preset].Filter.state = Control.getButton(Page::FILT, 0);

        Preset[preset].Filter.cutoff = Control.getKnob(Page::FILT, 0);
        Preset[preset].Filter.resonance = Control.getKnob(Page::FILT, 1);
        Preset[preset].Filter.punch = Control.getKnob(Page::FILT, 2);
        Preset[preset].Filter.type = Control.getKnob(Page::FILT, 3);

            Preset[preset].Filter.attack = Control.getKnob(Page::fENV, 0);
            Preset[preset].Filter.decay = Control.getKnob(Page::fENV, 1);
            Preset[preset].Filter.sustain = Control.getKnob(Page::fENV, 2);
            Preset[preset].Filter.release = Control.getKnob(Page::fENV, 3);

        Preset[preset].Modulation.state = Control.getButton(Page::LFO, 0);

        Preset[preset].Modulation.matrix = Control.getKnob(Page::LFO, 0);
        Preset[preset].Modulation.rate = Control.getKnob(Page::LFO, 1);
        Preset[preset].Modulation.depth = Control.getKnob(Page::LFO, 2);
        Preset[preset].Modulation.wave = Control.getKnob(Page::LFO, 3);

            Preset[preset].Effects.gain = Control.getKnob(Page::SHFT, 3);
            //
            //
            //

        Preset[preset].Arpeggiator.state = Control.getButton(Page::ARP, 0);

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
        Control.setButton(Page::FILT, 0, Preset[preset].Filter.state);

        Control.setKnob(Page::FILT, 0, Preset[preset].Filter.cutoff);
        Control.setKnob(Page::FILT, 1, Preset[preset].Filter.resonance);
        Control.setKnob(Page::FILT, 2, Preset[preset].Filter.punch);
        Control.setKnob(Page::FILT, 3, Preset[preset].Filter.type);

            Control.setKnob(Page::fENV, 0, Preset[preset].Filter.attack);
            Control.setKnob(Page::fENV, 1, Preset[preset].Filter.decay);
            Control.setKnob(Page::fENV, 2, Preset[preset].Filter.sustain);
            Control.setKnob(Page::fENV, 3, Preset[preset].Filter.release);

        Control.setButton(Page::LFO, 0, Preset[preset].Modulation.state);

        Control.setKnob(Page::LFO, 0, Preset[preset].Modulation.matrix);
        Control.setKnob(Page::LFO, 1, Preset[preset].Modulation.rate);
        Control.setKnob(Page::LFO, 2, Preset[preset].Modulation.depth);
        Control.setKnob(Page::LFO, 3, Preset[preset].Modulation.wave);

            Control.setKnob(Page::SHFT, 0, 0);
            Control.setKnob(Page::SHFT, 1, 0);
            Control.setKnob(Page::SHFT, 2, Preset[preset].Effects.gain);
            Control.setKnob(Page::SHFT, 3, 0);


        Control.setButton(Page::ARP, 0, Preset[preset].Arpeggiator.state);

        Control.setKnob(Page::ARP, 0, Preset[preset].Arpeggiator.gate);
        Control.setKnob(Page::ARP, 1, Preset[preset].Arpeggiator.divisions);
        Control.setKnob(Page::ARP, 2, Preset[preset].Arpeggiator.range);
        Control.setKnob(Page::ARP, 3, Preset[preset].Arpeggiator.direction);

            Control.setKnob(Page::sARP, 0, Preset[preset].Arpeggiator.rest);
            Control.setKnob(Page::sARP, 1, Preset[preset].Arpeggiator.bpm);
            Control.setKnob(Page::sARP, 2, Preset[preset].Arpeggiator.fMode);
            Control.setKnob(Page::sARP, 3, Preset[preset].Arpeggiator.octMode);


        Control.updateAll();

        LEDS::PRESET.preset(currentPreset);

        LEDS::FUNC1.set(Control.getButton(getPage(), 0));
        LEDS::FUNC2.set(Control.getButton(getPage(), 1));

        // needsUpdating = false; // this is here to try and stop lag on preset change as its currently being updated twice.
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

        LEDS::PRESET.flash(4,50);
    }
    
    // internal use only...
    void setPage (uint8_t page) {
        currentPage = page;

        PAGINATION::refresh();
        LEDS::PAGE_select(currentPage);

        LEDS::FUNC1.set(Control.getButton(getPage(), 0));
        LEDS::FUNC2.set(Control.getButton(getPage(), 1));

        resetShift();

        needsUpdating = true;
    }

    void changePage(void) {
        currentPage++;
        if (currentPage >= MAX_PAGES) currentPage = 0;

        PAGINATION::refresh();
        LEDS::PAGE_select(currentPage);

        LEDS::FUNC1.set(Control.getButton(getPage(), 0));
        LEDS::FUNC2.set(Control.getButton(getPage(), 1));

        resetShift();

        needsUpdating = true;
    }
    uint8_t getPage (void) {
        if (!shift) {
            return currentPage;
        } else {
            return currentPage + MAX_KNOBS;
        }
    }

    void setKnob (uint8_t page, uint8_t control, uint16_t input) {
        Control.setKnob(page, control, input);

        needsUpdating = true;
    }
    uint16_t getKnob (uint8_t page, uint8_t control) {
        return Control.getKnob(page, control);
    }
    

    void toggleButton1 (void) {
        // new code for page functions
        Control.toggleButton(getPage(), 0);
        LEDS::FUNC1.set(Control.getButton(getPage(), 0));
        needsUpdating = true;
    }
    void holdButton1 (void) {
        if (shift) {
            SYNTH::toggleNoise();
        } else {
            SYNTH::toggleSub();
        }
        LEDS::FUNC1.flash(4, 50);
    }

    // Remove??
    bool getButton1 () {
        return Control.getButton(Page::LFO, 0);
    }

    void toggleButton2 (void) {
        if (shift) {
            return; // return because the function is empty so it stops LED from toggling
        } else {
            Control.toggleButton(Page::ARP, 0);
            LEDS::FUNC2.set(Control.getButton(Page::ARP, 0));
            needsUpdating = true;
        }
    }
    void holdButton2 (void) {
        if (shift) {
            return; // return because the function is empty so it stops LED from flashing
        } else {
            ARP::toggleHold();
        }
        LEDS::FUNC2.flash(4,50);
    }

    // Remove??
    bool getButton2 () {
        return Control.getButton(Page::ARP, 0);
    }

    void setShift (bool input) {
        if (shift != input) {
        
            // TODO: Finesse this code as curently doesn't perform great, causing UI issues,
            shiftCounter++;
            if (shiftCounter >= SHIFT_TIMEOUT) {
                
                shift = !shift;
                needsUpdating = true;

                PAGINATION::refresh();
                LEDS::FUNC1.set(Control.getButton(getPage(), 0));
                LEDS::FUNC2.set(Control.getButton(getPage(), 1));

                shiftCounter = 0;
            }
        } else {
            shiftCounter = 0;
        }
    }

    // For internal use only - Stops page change from accidentally triggering a short shift page selection.
    void resetShift (void) {
        shift = false;
        shiftCounter = 0;
    }

    bool getShift (void) {
        return shift;
    }
    
    void update () {
        if (needsUpdating) {

            // Updates only active page, including whether we're in a shift page.
            Control.updatePage(getPage());
            
            needsUpdating = false;
        }
    }
    
}                                                                             