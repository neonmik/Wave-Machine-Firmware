#include "controls.h"

namespace CONTROLS {
    CONTROL Control;
    PRESET::SynthPreset Preset[MAX_PRESETS];
    PRESET::SynthPreset activePreset;

    void init () {
        PAGINATION::init();

        PRESET::init();

        currentPreset = DEFAULT_PRESET;
        
        for (int i = 0; i < MAX_PRESETS; i++) {
            PRESET::load(i, Preset[i]);
        }

        loadPresetFromSlot(currentPreset);



        setPage(currentPage);
    }

    void exportPresetTest (void) {
        PRESET::exportViaSysEx(Preset[0]);
    }
    
    void setupButtonAssignment (void) {
        Buttons::PAGE.setShortPressAction(&CONTROLS::changePage);
        Buttons::FUNC1.setShortPressAction(&CONTROLS::toggleButton1);
        Buttons::FUNC1.setLongPressAction(&CONTROLS::holdButton1);
        Buttons::FUNC2.setShortPressAction(&CONTROLS::toggleButton2);
        Buttons::FUNC2.setLongPressAction(&CONTROLS::holdButton2);
        Buttons::PRESET.setShortPressAction(&CONTROLS::changePreset);
    }

    void setPreset (uint8_t preset) {
        currentPreset = preset;

        loadPresetFromSlot(currentPreset);

        PAGINATION::refresh();
    }

    void changePreset (void) {
        if (!shift) {
            ++currentPreset;
            if (currentPreset >= MAX_PRESETS) currentPreset = 0;

            loadPresetFromSlot(currentPreset);
            setPage(currentPage);
        } else {
            save();
        }
    }
    
    uint8_t getPreset () {
        return currentPreset;
    }
    
    void applyPresetToControls (PRESET::SynthPreset &preset) {
        Control.setKnob(Page::MAIN, 0, preset.Voice.Oscillator1.shape);
        Control.setKnob(Page::MAIN, 1, preset.Voice.Oscillator1.vector);
        Control.setKnob(Page::MAIN, 2, preset.Voice.octave);
        Control.setKnob(Page::MAIN, 3, preset.Voice.pitchBend);

            Control.setKnob(Page::ADSR, 0, preset.Voice.Envelope.attack);
            Control.setKnob(Page::ADSR, 1, preset.Voice.Envelope.decay);
            Control.setKnob(Page::ADSR, 2, preset.Voice.Envelope.sustain);
            Control.setKnob(Page::ADSR, 3, preset.Voice.Envelope.release);

        Control.setButton(Page::FILT, 0, preset.States.stateFLT);

        Control.setKnob(Page::FILT, 0, preset.Filter.cutoff);
        Control.setKnob(Page::FILT, 1, preset.Filter.resonance);
        Control.setKnob(Page::FILT, 2, preset.Filter.punch);
        Control.setKnob(Page::FILT, 3, preset.Filter.type);

            Control.setKnob(Page::fENV, 0, preset.Filter.Envelope.attack);
            Control.setKnob(Page::fENV, 1, preset.Filter.Envelope.decay);
            Control.setKnob(Page::fENV, 2, preset.Filter.Envelope.sustain);
            Control.setKnob(Page::fENV, 3, preset.Filter.Envelope.release);

        Control.setButton(Page::LFO, 0, preset.States.stateLFO);

        Control.setKnob(Page::LFO, 0, preset.Modulation.matrix);
        Control.setKnob(Page::LFO, 1, preset.Modulation.rate);
        Control.setKnob(Page::LFO, 2, preset.Modulation.depth);
        Control.setKnob(Page::LFO, 3, preset.Modulation.wave);

            Control.setKnob(Page::SHFT, 0, preset.Voice.Oscillator1.shape);
            Control.setKnob(Page::SHFT, 1, preset.Voice.noiseLevel);
            Control.setKnob(Page::SHFT, 2, preset.Effects.gain);
            Control.setKnob(Page::SHFT, 3, preset.Voice.detune);

        Control.setButton(Page::ARP, 0, preset.States.stateARP);

        Control.setKnob(Page::ARP, 0, preset.Arpeggiator.gate);
        Control.setKnob(Page::ARP, 1, preset.Arpeggiator.divisions);
        Control.setKnob(Page::ARP, 2, preset.Arpeggiator.range);
        Control.setKnob(Page::ARP, 3, preset.Arpeggiator.direction);

            Control.setKnob(Page::sARP, 0, preset.Arpeggiator.playedOrder);
            Control.setKnob(Page::sARP, 1, preset.Arpeggiator.bpm);
            Control.setKnob(Page::sARP, 2, preset.Filter.triggerMode);
            Control.setKnob(Page::sARP, 3, preset.Arpeggiator.octaveMode);
    }
    void extractPresetFromControls (PRESET::SynthPreset &preset) {

        preset.Voice.Oscillator1.shape = Control.getKnob(Page::MAIN, 0);
        preset.Voice.Oscillator1.vector = Control.getKnob(Page::MAIN, 1);
        preset.Voice.octave = Control.getKnob(Page::MAIN, 2);
        preset.Voice.pitchBend = Control.getKnob(Page::MAIN, 3);

            preset.Voice.Envelope.attack = Control.getKnob(Page::ADSR, 0);
            preset.Voice.Envelope.decay = Control.getKnob(Page::ADSR, 1);
            preset.Voice.Envelope.sustain = Control.getKnob(Page::ADSR, 2);
            preset.Voice.Envelope.release = Control.getKnob(Page::ADSR, 3);

        preset.States.stateFLT = Control.getButton(Page::FILT, 0);

        preset.Filter.cutoff = Control.getKnob(Page::FILT, 0);
        preset.Filter.resonance = Control.getKnob(Page::FILT, 1);
        preset.Filter.punch = Control.getKnob(Page::FILT, 2);
        preset.Filter.type = Control.getKnob(Page::FILT, 3);

            preset.Filter.Envelope.attack = Control.getKnob(Page::fENV, 0);
            preset.Filter.Envelope.decay = Control.getKnob(Page::fENV, 1);
            preset.Filter.Envelope.sustain = Control.getKnob(Page::fENV, 2);
            preset.Filter.Envelope.release = Control.getKnob(Page::fENV, 3);

        preset.States.stateLFO = Control.getButton(Page::LFO, 0);

        preset.Modulation.matrix = Control.getKnob(Page::LFO, 0);
        preset.Modulation.rate = Control.getKnob(Page::LFO, 1);
        preset.Modulation.depth = Control.getKnob(Page::LFO, 2);
        preset.Modulation.wave = Control.getKnob(Page::LFO, 3);

            preset.Voice.Oscillator2.shape = Control.getKnob(Page::SHFT, 0);
            preset.Voice.noiseLevel = Control.getKnob(Page::SHFT, 1);
            preset.Effects.gain = Control.getKnob(Page::SHFT, 2);
            preset.Voice.detune = Control.getKnob(Page::SHFT, 3);

        preset.States.stateARP = Control.getButton(Page::ARP, 0);

        preset.Arpeggiator.gate = Control.getKnob(Page::ARP, 0);
        preset.Arpeggiator.divisions = Control.getKnob(Page::ARP, 1);
        preset.Arpeggiator.range = Control.getKnob(Page::ARP, 2);
        preset.Arpeggiator.direction = Control.getKnob(Page::ARP, 3);

            preset.Arpeggiator.playedOrder = Control.getKnob(Page::sARP, 0);
            preset.Arpeggiator.bpm = Control.getKnob(Page::sARP, 1);
            preset.Filter.triggerMode = Control.getKnob(Page::sARP, 2);
            preset.Arpeggiator.octaveMode = Control.getKnob(Page::sARP, 3);
    }

   
    void savePresetToSlot (uint8_t slot) {
        Preset[slot] = activePreset;

        extractPresetFromControls(Preset[slot]);
        
        PRESET::save(slot, Preset[slot]);
        
    }

    void loadPresetFromSlot (uint8_t slot) {
        activePreset = Preset[slot];

        applyPresetToControls(activePreset);

        Control.updateAll();

        LEDS::PRESET.preset(currentPreset);

        refreshInterface();
    }
    
    
    void exportPresets(void) {
        PRESET::SynthPreset export_buffer[MAX_PRESETS];

        for (int i = 0; i < MAX_PRESETS; i++) {
            // EEPROM::loadPreset(i, export_buffer[i]);
            PRESET::load(i, export_buffer[i]);
        }

        // send export_buffer somewhere?    - maybe save as a file that can be accessed in USB?
        //                                  - send over MIDI
    }

    // Function for restoring the Factory Presets from the EEPROM to the main preset storage area.
    void factoryRestore (void) {

        printf("\nFactory Restore in progress!\n");

        for (int i = 0; i < MAX_PRESETS; i++) {
            // EEPROM::restoreFactoryPreset(i);
            PRESET::factoryRestore(i);
        }
        
        printf("Factory Settings restored!\n\n");
        
        setPreset(currentPreset); // not sure if this needs to be here? just needs to make sure it updates the settings right after factory restore, seems like the safest way to do it
    }

    // Function for writing current Presets to the Factory Preset storage area
    void updateFactoryPresets (void) {

        printf("Storing currrent presets to Factory Preset slots!\n");

        for (int i = 0; i < MAX_PRESETS; i++) {
            // EEPROM::writeFactoryPreset(i);
            PRESET::factoryWrite(i);
        }

        printf("All Presets backed up!\n\n");
    }

    // Save current Preset
    void save () {
        savePresetToSlot(currentPreset);

        LEDS::PRESET.flash(4,8);
    }
    
    // internal use only...
    void setPage (uint8_t page) {
        resetShift();
        
        currentPage = page;

        LEDS::PAGE_select(currentPage);
        refreshInterface();

        needsUpdating = true;
    }

    void changePage(void) {
        resetShift();
        
        currentPage++;
        if (currentPage >= MAX_PAGES) currentPage = 0;

        LEDS::PAGE_select(currentPage);

        refreshInterface();
        
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
        if (!shift) {
            // Normal Function
            Control.toggleButton(Page::LFO, 0);
            LEDS::FUNC1.set(Control.getButton(Page::LFO, 0));
            return; // no function, so skip the LED update
        } else {
            // Shift Function
            Control.toggleButton(Page::FILT, 0);
            LEDS::FUNC1.set(Control.getButton(Page::FILT, 0));
            return; // no function, so skip the LED update
        }
        // Control.toggleButton(getPage(), 0);
        // LEDS::FUNC1.set(Control.getButton(getPage(), 0));
        needsUpdating = true;
    }
    void holdButton1 (void) {
        if (!shift) {
            // Normal Function
            return; // no function, so skip the LED flash
        } else {
            // Shift Function
            return; // no function, so skip the LED flash
        }
        LEDS::FUNC1.flash(4, LEDS::Speed::NORMAL);
    }

    void toggleButton2 (void) {
        if (!shift) {
            // Normal Function
            Control.toggleButton(Page::ARP, 0);
            LEDS::FUNC2.set(Control.getButton(Page::ARP, 0));
            // return; // no function, so skip the LED update
        } else {
            // Shift Function
            ARP::toggleHold();
            LEDS::FUNC2.set(ARP::getHold());
            LEDS::FUNC2.flash(4, LEDS::Speed::NORMAL); // currently blocks indefinite flash.
            // return; // no function, so skip the LED update
        }
        // Control.toggleButton(getPage(), 1);
        // LEDS::FUNC2.set(Control.getButton(getPage(), 1));
        needsUpdating = true;
    }
    void holdButton2 (void) {
        if (shift) {
            return; // return because the function is empty so it stops LED from flashing
        } else {
            // ARP::toggleHold();
            return;
        }
        LEDS::FUNC2.flash(4, LEDS::Speed::NORMAL); // currently blocks indefinite flash.
    }

    void setShift (bool input) {
        if (shift != input) {
        
            // TODO: Finesse this code as curently doesn't perform great, causing UI issues
            shiftCounter++;
            if (shiftCounter >= SHIFT_TIMEOUT) {
                
                shift = !shift;
                needsUpdating = true;
                shiftCounter = 0;

                refreshInterface();
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
        
        controlRateCounter++;
        
        if (controlRateCounter >= 10) {
            controlRateCounter = 0;
            
            if (!needsUpdating) 
                return;

            Control.update(getPage());
            needsUpdating = false;
        }
    }

    void refreshInterface (void) {
        PAGINATION::refresh();
        if (!shift) {
            // Normal functions:
            LEDS::FUNC1.set(Control.getButton(Page::LFO, 0));
            LEDS::FUNC2.set(Control.getButton(Page::ARP, 0));
        } else {
            LEDS::FUNC1.set(Control.getButton(Page::FILT, 0));
            LEDS::FUNC2.set(ARP::getHold());
        }
    }
    
}                                                                             