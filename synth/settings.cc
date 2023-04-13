#include "settings.h"

namespace SETTINGS {
    PRESET Preset[MAX_PRESETS];
    CONTROL Control;

    void init () {
        _preset = _default_preset;
        load_preset();
    }

    void set_preset (uint8_t preset) {
        // save_preset();
        _preset = preset;
        load_preset();
        _changed = true;
    }
    uint8_t get_preset () {
        return _preset;
    }
    void save_preset () {

        Preset[_preset].Wave.shape = Control.get(0, 0);
        Preset[_preset].Wave.vector = Control.get(0, 1);
        Preset[_preset].Wave.octave = Control.get(0, 2);
        Preset[_preset].Wave.pitch = Control.get(0, 3);

        Preset[_preset].Envelope.attack = Control.get(1, 0);
        Preset[_preset].Envelope.decay = Control.get(1, 1);
        Preset[_preset].Envelope.sustain = Control.get(1, 2);
        Preset[_preset].Envelope.release = Control.get(1, 3);

        Preset[_preset].Modulation.state = Control.get_lfo();
        Preset[_preset].Modulation.matrix = Control.get(2, 0);
        Preset[_preset].Modulation.rate = Control.get(2, 1);
        Preset[_preset].Modulation.depth = Control.get(2, 2);
        Preset[_preset].Modulation.wave = Control.get(2, 3);

        Preset[_preset].Arpeggiator.state = Control.get_arp();
        Preset[_preset].Arpeggiator.hold = Control.get(3, 0);
        Preset[_preset].Arpeggiator.divisisions = Control.get(3, 1);
        Preset[_preset].Arpeggiator.range = Control.get(3, 2);
        Preset[_preset].Arpeggiator.direction = Control.get(3, 3);
        
        // code here for saving preset infomation to flash
        // flash_write(Preset[_preset]);
    }
    void load_preset () {
        // code here for loading preset infomation from flash
        // Preset[_preset] = flash_read();

        Control.set(0, 0, Preset[_preset].Wave.shape);
        Control.set(0, 1, Preset[_preset].Wave.vector);
        Control.set(0, 2, Preset[_preset].Wave.octave);
        Control.set(0, 3, Preset[_preset].Wave.pitch);

        Control.set(1, 0, Preset[_preset].Envelope.attack);
        Control.set(1, 1, Preset[_preset].Envelope.decay);
        Control.set(1, 2, Preset[_preset].Envelope.sustain);
        Control.set(1, 3, Preset[_preset].Envelope.release);

        Control.set_lfo(Preset[_preset].Modulation.state);
        Control.set(2, 0, Preset[_preset].Modulation.matrix);
        Control.set(2, 1, Preset[_preset].Modulation.rate);
        Control.set(2, 2, Preset[_preset].Modulation.depth);
        Control.set(2, 3, Preset[_preset].Modulation.wave);

        Control.set_arp(Preset[_preset].Arpeggiator.state);
        Control.set(3, 0, Preset[_preset].Arpeggiator.hold);
        Control.set(3, 1, Preset[_preset].Arpeggiator.divisisions);
        Control.set(3, 2, Preset[_preset].Arpeggiator.range);
        Control.set(3, 3, Preset[_preset].Arpeggiator.direction);

        _changed = true;
    
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