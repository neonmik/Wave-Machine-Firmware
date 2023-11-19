#include "controls.h"

namespace CONTROLS {
    CONTROL Control;
    PRESET Preset[MAX_PRESETS];
    PRESET activePreset;

    void init () {
        PAGINATION::init();

        EEPROM::init();

        currentPreset = DEFAULT_PRESET;
        
        // factoryRestore();
        Buttons::PAGE.setShortPressAction(&CONTROLS::changePage);
        Buttons::FUNC1.setShortPressAction(&CONTROLS::toggleButton1);
        Buttons::FUNC1.setLongPressAction(&CONTROLS::holdButton1);
        Buttons::FUNC2.setShortPressAction(&CONTROLS::toggleButton2);
        Buttons::FUNC2.setShortPressAction(&CONTROLS::holdButton2);
        Buttons::PRESET.setShortPressAction(&CONTROLS::changePreset);
        
        for (int i = 0; i < MAX_PRESETS; i++) {
            EEPROM::loadPreset(i, Preset[i]);
        }

        loadPreset(currentPreset);

        setPage(currentPage);
    }

    void setPreset (uint8_t preset) {
        currentPreset = preset;

        loadPreset(currentPreset);

        PAGINATION::refresh();
    }
    void changePreset (void) {
        if (!shift) {
            ++currentPreset;
            if (currentPreset >= MAX_PRESETS) currentPreset = 0;

            loadPreset(currentPreset);

            PAGINATION::refresh();
        } else {
            save();
        }
    }
    
    uint8_t getPreset () {
        return currentPreset;
    }
    void applyPreset (PRESET &preset) {
        Control.setKnob(Page::MAIN, 0, preset.Wave.shape);
        Control.setKnob(Page::MAIN, 1, preset.Wave.vector);
        Control.setKnob(Page::MAIN, 2, preset.Wave.octave);
        Control.setKnob(Page::MAIN, 3, preset.Wave.pitch);

            Control.setKnob(Page::ADSR, 0, preset.Envelope.attack);
            Control.setKnob(Page::ADSR, 1, preset.Envelope.decay);
            Control.setKnob(Page::ADSR, 2, preset.Envelope.sustain);
            Control.setKnob(Page::ADSR, 3, preset.Envelope.release);

        Control.setButton(Page::FILT, 0, preset.Filter.state);

        Control.setKnob(Page::FILT, 0, preset.Filter.cutoff);
        Control.setKnob(Page::FILT, 1, preset.Filter.resonance);
        Control.setKnob(Page::FILT, 2, preset.Filter.punch);
        Control.setKnob(Page::FILT, 3, preset.Filter.type);

            Control.setKnob(Page::fENV, 0, preset.Filter.attack);
            Control.setKnob(Page::fENV, 1, preset.Filter.decay);
            Control.setKnob(Page::fENV, 2, preset.Filter.sustain);
            Control.setKnob(Page::fENV, 3, preset.Filter.release);

        Control.setButton(Page::LFO, 0, preset.Modulation.state);

        Control.setKnob(Page::LFO, 0, preset.Modulation.matrix);
        Control.setKnob(Page::LFO, 1, preset.Modulation.rate);
        Control.setKnob(Page::LFO, 2, preset.Modulation.depth);
        Control.setKnob(Page::LFO, 3, preset.Modulation.wave);

            Control.setKnob(Page::SHFT, 0, 0);
            Control.setKnob(Page::SHFT, 1, 0);
            Control.setKnob(Page::SHFT, 2, preset.Effects.gain);
            Control.setKnob(Page::SHFT, 3, 0);

        Control.setButton(Page::ARP, 0, preset.Arpeggiator.state);

        Control.setKnob(Page::ARP, 0, preset.Arpeggiator.gate);
        Control.setKnob(Page::ARP, 1, preset.Arpeggiator.divisions);
        Control.setKnob(Page::ARP, 2, preset.Arpeggiator.range);
        Control.setKnob(Page::ARP, 3, preset.Arpeggiator.direction);

            Control.setKnob(Page::sARP, 0, preset.Arpeggiator.rest);
            Control.setKnob(Page::sARP, 1, preset.Arpeggiator.bpm);
            Control.setKnob(Page::sARP, 2, preset.Arpeggiator.fMode);
            Control.setKnob(Page::sARP, 3, preset.Arpeggiator.octMode);
    }
    void pullPreset (PRESET &preset) {
        preset.Wave.shape = Control.getKnob(Page::MAIN, 0);
        preset.Wave.vector = Control.getKnob(Page::MAIN, 1);
        preset.Wave.octave = Control.getKnob(Page::MAIN, 2);
        preset.Wave.pitch = Control.getKnob(Page::MAIN, 3);

            preset.Envelope.attack = Control.getKnob(Page::ADSR, 0);
            preset.Envelope.decay = Control.getKnob(Page::ADSR, 1);
            preset.Envelope.sustain = Control.getKnob(Page::ADSR, 2);
            preset.Envelope.release = Control.getKnob(Page::ADSR, 3);

        preset.Filter.state = Control.getButton(Page::FILT, 0);

        preset.Filter.cutoff = Control.getKnob(Page::FILT, 0);
        preset.Filter.resonance = Control.getKnob(Page::FILT, 1);
        preset.Filter.punch = Control.getKnob(Page::FILT, 2);
        preset.Filter.type = Control.getKnob(Page::FILT, 3);

            preset.Filter.attack = Control.getKnob(Page::fENV, 0);
            preset.Filter.decay = Control.getKnob(Page::fENV, 1);
            preset.Filter.sustain = Control.getKnob(Page::fENV, 2);
            preset.Filter.release = Control.getKnob(Page::fENV, 3);

        preset.Modulation.state = Control.getButton(Page::LFO, 0);

        preset.Modulation.matrix = Control.getKnob(Page::LFO, 0);
        preset.Modulation.rate = Control.getKnob(Page::LFO, 1);
        preset.Modulation.depth = Control.getKnob(Page::LFO, 2);
        preset.Modulation.wave = Control.getKnob(Page::LFO, 3);

            preset.Effects.gain = Control.getKnob(Page::SHFT, 3);
            //
            //
            //

        preset.Arpeggiator.state = Control.getButton(Page::ARP, 0);

        preset.Arpeggiator.gate = Control.getKnob(Page::ARP, 0);
        preset.Arpeggiator.divisions = Control.getKnob(Page::ARP, 1);
        preset.Arpeggiator.range = Control.getKnob(Page::ARP, 2);
        preset.Arpeggiator.direction = Control.getKnob(Page::ARP, 3);

            preset.Arpeggiator.rest = Control.getKnob(Page::sARP, 0);
            preset.Arpeggiator.bpm = Control.getKnob(Page::sARP, 1);
            preset.Arpeggiator.fMode = Control.getKnob(Page::sARP, 2);
            preset.Arpeggiator.octMode = Control.getKnob(Page::sARP, 3);
    }
    
    void savePreset (uint8_t slot) {
        Preset[slot] = activePreset;

        pullPreset(Preset[slot]);

        EEPROM::savePreset(slot, Preset[slot]);
        
    }

    void loadPreset (uint8_t slot) {
        // Moved this to the init code to save on processing time between presets.
        // EEPROM::loadPreset(slot, Preset[slot]);

        activePreset = Preset[slot];

        applyPreset(activePreset);

        Control.updateAll();

        LEDS::PRESET.preset(currentPreset);

        LEDS::FUNC1.set(Control.getButton(getPage(), 0));
        LEDS::FUNC2.set(Control.getButton(getPage(), 1));

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

        LEDS::PRESET.flash(4,8);
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

        resetShift();
        
        currentPage++;
        if (currentPage >= MAX_PAGES) currentPage = 0;

        PAGINATION::refresh();
        LEDS::PAGE_select(currentPage);

        LEDS::FUNC1.set(Control.getButton(getPage(), 0));
        LEDS::FUNC2.set(Control.getButton(getPage(), 1));


        // needsUpdating = true;
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
        LEDS::FUNC1.flash(4, LEDS::Speed::NORMAL);
    }

    // Remove??
    bool getButton1 () {
        return Control.getButton(Page::LFO, 0);
    }

    void toggleButton2 (void) {
        Control.toggleButton(getPage(), 1);
        LEDS::FUNC2.set(Control.getButton(getPage(), 1));
        needsUpdating = true;
    }
    void holdButton2 (void) {
        if (shift) {
            return; // return because the function is empty so it stops LED from flashing
        } else {
            ARP::toggleHold();
        }
        LEDS::FUNC2.flash(4, LEDS::Speed::NORMAL);
    }

    // Remove??
    bool getButton2 () {
        return Control.getButton(Page::ARP, 0);
    }

    void setShift (bool input) {
        if (shift != input) {
        
            // TODO: Finesse this code as curently doesn't perform great, causing UI issues
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
        CONTROLS::setShift(Buttons::PAGE.get(Buttons::State::SHIFT));
        
        if (needsUpdating) {

            // Updates only active page, including whether we're in a shift page.
            Control.update(getPage());
            
            needsUpdating = false;
        }
    }
    
}                                                                             