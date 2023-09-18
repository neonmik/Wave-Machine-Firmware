#include "controls.h"

namespace CONTROLS {
    CONTROL Control;
    PRESET Preset[MAX_PRESETS];

    void Init () {
        EEPROM::Init();

        _preset = DEFAULT_PRESET;
        load_preset(_preset);

    }

    void set_preset (uint8_t preset) {
        _preset = preset;

        load_preset(_preset);
    }
    
    uint8_t get_preset () {
        return _preset;
    }

    void save_preset (uint8_t preset) {

        Preset[preset].Wave.shape = Control.get(Controls::MAIN, 0);
        Preset[preset].Wave.vector = Control.get(Controls::MAIN, 1);
        Preset[preset].Wave.octave = Control.get(Controls::MAIN, 2);
        Preset[preset].Wave.pitch = Control.get(Controls::MAIN, 3);

            Preset[preset].Envelope.attack = Control.get(Controls::ADSR, 0);
            Preset[preset].Envelope.decay = Control.get(Controls::ADSR, 1);
            Preset[preset].Envelope.sustain = Control.get(Controls::ADSR, 2);
            Preset[preset].Envelope.release = Control.get(Controls::ADSR, 3);

        Preset[preset].Filter.state = true;

        Preset[preset].Filter.cutoff = Control.get(Controls::FILT, 0);
        Preset[preset].Filter.resonance = Control.get(Controls::FILT, 1);
        Preset[preset].Filter.punch = Control.get(Controls::FILT, 2);
        Preset[preset].Filter.type = Control.get(Controls::FILT, 3);

            Preset[preset].Filter.attack = Control.get(Controls::fENV, 0);
            Preset[preset].Filter.decay = Control.get(Controls::fENV, 1);
            Preset[preset].Filter.sustain = Control.get(Controls::fENV, 2);
            Preset[preset].Filter.release = Control.get(Controls::fENV, 3);

        Preset[preset].Modulation.state = Control.get_lfo();

        Preset[preset].Modulation.matrix = Control.get(Controls::MOD, 0);
        Preset[preset].Modulation.rate = Control.get(Controls::MOD, 1);
        Preset[preset].Modulation.depth = Control.get(Controls::MOD, 2);
        Preset[preset].Modulation.wave = Control.get(Controls::MOD, 3);


            Preset[preset].Effects.gain = Control.get(Controls::SHFT, 3);

        Preset[preset].Arpeggiator.state = Control.get_arp();

        Preset[preset].Arpeggiator.gate = Control.get(Controls::ARP, 0);
        Preset[preset].Arpeggiator.divisions = Control.get(Controls::ARP, 1);
        Preset[preset].Arpeggiator.range = Control.get(Controls::ARP, 2);
        Preset[preset].Arpeggiator.direction = Control.get(Controls::ARP, 3);

            Preset[preset].Arpeggiator.rest = Control.get(Controls::sARP, 0);
            Preset[preset].Arpeggiator.bpm = Control.get(Controls::sARP, 1);
            Preset[preset].Arpeggiator.fMode = Control.get(Controls::sARP, 2);
            Preset[preset].Arpeggiator.octMode = Control.get(Controls::sARP, 3);


        
        EEPROM::savePreset(preset, Preset[preset]);
    }
    void load_preset (uint8_t preset) {
        PRESET temp;
        
        // Preset[preset] = temp;
        EEPROM::loadPreset(preset, Preset[preset]);

        Control.set(Controls::MAIN, 0, Preset[preset].Wave.shape);
        Control.set(Controls::MAIN, 1, Preset[preset].Wave.vector);
        Control.set(Controls::MAIN, 2, Preset[preset].Wave.octave);
        Control.set(Controls::MAIN, 3, Preset[preset].Wave.pitch);

            Control.set(Controls::ADSR, 0, Preset[preset].Envelope.attack);
            Control.set(Controls::ADSR, 1, Preset[preset].Envelope.decay);
            Control.set(Controls::ADSR, 2, Preset[preset].Envelope.sustain);
            Control.set(Controls::ADSR, 3, Preset[preset].Envelope.release);

        // Control.set_filter(Preset[preset].Filter.state);

        Control.set(Controls::FILT, 0, Preset[preset].Filter.cutoff);
        Control.set(Controls::FILT, 1, Preset[preset].Filter.resonance);
        Control.set(Controls::FILT, 2, Preset[preset].Filter.punch);
        Control.set(Controls::FILT, 3, Preset[preset].Filter.type);

            Control.set(Controls::fENV, 0, Preset[preset].Filter.attack);
            Control.set(Controls::fENV, 1, Preset[preset].Filter.decay);
            Control.set(Controls::fENV, 2, Preset[preset].Filter.sustain);
            Control.set(Controls::fENV, 3, Preset[preset].Filter.release);


        Control.set_lfo(Preset[preset].Modulation.state);
        Control.set(Controls::MOD, 0, Preset[preset].Modulation.matrix);
        Control.set(Controls::MOD, 1, Preset[preset].Modulation.rate);
        Control.set(Controls::MOD, 2, Preset[preset].Modulation.depth);
        Control.set(Controls::MOD, 3, Preset[preset].Modulation.wave);

            Control.set(Controls::SHFT, 0, 0);
            Control.set(Controls::SHFT, 1, 0);
            Control.set(Controls::SHFT, 2, Preset[preset].Effects.gain);
            Control.set(Controls::SHFT, 3, 0);


        Control.set_arp(Preset[preset].Arpeggiator.state);
        Control.set(Controls::ARP, 0, Preset[preset].Arpeggiator.gate);
        Control.set(Controls::ARP, 1, Preset[preset].Arpeggiator.divisions);
        Control.set(Controls::ARP, 2, Preset[preset].Arpeggiator.range);
        Control.set(Controls::ARP, 3, Preset[preset].Arpeggiator.direction);

            Control.set(Controls::sARP, 0, Preset[preset].Arpeggiator.rest);
            Control.set(Controls::sARP, 0, Preset[preset].Arpeggiator.bpm);
            Control.set(Controls::sARP, 0, Preset[preset].Arpeggiator.fMode);
            Control.set(Controls::sARP, 0, Preset[preset].Arpeggiator.octMode);
        
        // Control.set(Controls::SPAR, 0, 0);
        // Control.set(Controls::SPAR, 1, 0);
        // Control.set(Controls::SPAR, 2, 0);
        // Control.set(Controls::SPAR, 3, 0);

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

    // Function for restoring the Factory Presets from the EEPROM to the main preset storage area.
    void factory_restore (void) {

        printf("\nFactory Restore in progress!\n");

        for (int i = 0; i < MAX_PRESETS; i++) {
            EEPROM::restoreFactoryPreset(i);
        }
        
        printf("Factory Settings restored!\n\n");
        
        set_preset(_preset); // not sure if this needs to be here? just needs to make sure it updates the settings right after factory restore, seems like the safest way to do it
    }

    // Function for writing current Presets to the Factory Preset storage area
    void write_factory_presets (void) {

        printf("Storing currrent presets to Factory Preset slots!\n");

        for (int i = 0; i < MAX_PRESETS; i++) {
            EEPROM::writeFactoryPreset(i);
        }

        printf("All Presets backed up!\n\n");


    }

    // Save current Preset
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

    void toggle_shift (void) {
        _changed = true;
        _shift != _shift;
    }
    bool get_shift (void) {
        return _shift;
    }
    
    void Update () {
        if (_changed) {
            Control.Update();
            
            _changed = false;
        }
    }
    
}                                                                             