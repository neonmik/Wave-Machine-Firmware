#include "settings.h"

namespace SETTINGS {
    CONTROL Control;
    PRESET Preset[MAX_PRESETS];

    void init () {
        EEPROM::init();

        _preset = _default_preset;
        load_preset(_preset);
    }

    void set_preset (uint8_t preset) {
        // this is commented because I don't want it to save the preset setting without us doing it on purpose
        // save_preset();
        _preset = preset;
        load_preset(_preset);
        ARP::reset();
        _changed = true;
    }
    uint8_t get_preset () {
        return _preset;
    }
    void save_preset (uint8_t preset) {

        Preset[preset].Wave.shape = Control.get(0, 0);
        Preset[preset].Wave.vector = Control.get(0, 1);
        Preset[preset].Wave.octave = Control.get(0, 2);
        Preset[preset].Wave.pitch = Control.get(0, 3);

        Preset[preset].Envelope.attack = Control.get(1, 0);
        Preset[preset].Envelope.decay = Control.get(1, 1);
        Preset[preset].Envelope.sustain = Control.get(1, 2);
        Preset[preset].Envelope.release = Control.get(1, 3);

        Preset[preset].Modulation.state = Control.get_lfo();
        Preset[preset].Modulation.matrix = Control.get(2, 0);
        Preset[preset].Modulation.rate = Control.get(2, 1);
        Preset[preset].Modulation.depth = Control.get(2, 2);
        Preset[preset].Modulation.wave = Control.get(2, 3);

        Preset[preset].Arpeggiator.state = Control.get_arp();
        Preset[preset].Arpeggiator.hold = Control.get(3, 0);
        Preset[preset].Arpeggiator.divisions = Control.get(3, 1);
        Preset[preset].Arpeggiator.range = Control.get(3, 2);
        Preset[preset].Arpeggiator.direction = Control.get(3, 3);
        
        EEPROM::savePreset(preset, Preset[preset]);
    }
    void load_preset (uint8_t preset) {

        EEPROM::loadPreset(preset, Preset[preset]);

        Control.set(0, 0, Preset[preset].Wave.shape);
        Control.set(0, 1, Preset[preset].Wave.vector);
        Control.set(0, 2, Preset[preset].Wave.octave);
        Control.set(0, 3, Preset[preset].Wave.pitch);

        Control.set(1, 0, Preset[preset].Envelope.attack);
        Control.set(1, 1, Preset[preset].Envelope.decay);
        Control.set(1, 2, Preset[preset].Envelope.sustain);
        Control.set(1, 3, Preset[preset].Envelope.release);

        Control.set_lfo(Preset[preset].Modulation.state);
        Control.set(2, 0, Preset[preset].Modulation.matrix);
        Control.set(2, 1, Preset[preset].Modulation.rate);
        Control.set(2, 2, Preset[preset].Modulation.depth);
        Control.set(2, 3, Preset[preset].Modulation.wave);

        Control.set_arp(Preset[preset].Arpeggiator.state);
        Control.set(3, 0, Preset[preset].Arpeggiator.hold);
        Control.set(3, 1, Preset[preset].Arpeggiator.divisions);
        Control.set(3, 2, Preset[preset].Arpeggiator.range);
        Control.set(3, 3, Preset[preset].Arpeggiator.direction);

        _changed = true;
    
    }
    void export_presets(void) {
        PRESET export_buffer[MAX_PRESETS];

        for (int i = 0; i < MAX_PRESETS; i++) {
            EEPROM::loadPreset(i, export_buffer[i]);
        }

        // send export_buffer somewhere?    - maybe save as a file that can be accessed in USB?
        //                                  - send over MIDI
    }
    void factory_restore (void) {

        printf("Factory Restore in progress!\n");
        
        for (int i = 0; i < MAX_PRESETS; i++) {
            EEPROM::restorePreset(i);
        }
        printf("Factory Settings restored!\n");
        
        set_preset(_preset); // not sure if this needs to be here? just needs to make sure it updates the settings right after factory restore, seems like the safest way to do it
    }
    void save () {
        save_preset(_preset);
    }

    void set_page (uint8_t page) {
        _changed = true;
        _page = page;
    }
    uint8_t get_page () {
        return _page;
    }

    void set_value (uint8_t page, uint8_t control, uint16_t input) {
        _changed = true;
        Control.set(page, control, input);
    }
    uint16_t get_value (uint8_t page, uint8_t control) {
        return Control.get(page, control);
    }
       
    void toggle_lfo () {
        _changed = true;
        Control.toggle_lfo();
    }
    bool get_lfo () {
        return Control.get_lfo();
    }

    void toggle_arp () {
        _changed = true;
        Control.toggle_arp();
    }
    bool get_arp () {
        return Control.get_arp();
    }
    
    void update () {
        if (_changed) {
            Control.update();
            
            _changed = false;
        }
    }
    
}                                                                             