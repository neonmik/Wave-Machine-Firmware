#include "settings.h"

namespace SETTINGS {
    PRESET Preset[8];
    void init () {
        fetch();
    }
    void set_preset (uint8_t preset) {
        _changed = true;
        _preset = preset;
    }
    uint8_t get_preset (void) {
        return _preset;
    }

    void set_page (uint8_t page) {
        _page = page;
    }
    uint8_t get_page (void) {
        return _page;
    }

    void set_value (uint8_t page, uint8_t control, uint16_t input) {
        _changed = true;
        Preset[_preset].set(page, control, input);
    }
    uint16_t get_value (uint8_t page, uint8_t control) {
        return Preset[_preset].get(page, control);
    }
    
    
    void toggle_lfo (void) {
        _changed = true;
        Preset[_preset].toggle_lfo();
    }
    bool get_lfo (void) {
        return Preset[_preset].get_lfo();
    }

    void toggle_arp (void) {
        _changed = true;
        Preset[_preset].toggle_arp();
    }
    bool get_arp (void) {
        return Preset[_preset].get_arp();
    }
    
    void fetch () {
        Preset[0].fetch();
        Preset[1].fetch();
        Preset[2].fetch();
        Preset[3].fetch();
        Preset[4].fetch();
        Preset[5].fetch();
        Preset[6].fetch();
        Preset[7].fetch();
    }
    void update () {
        if (_changed) {
             Preset[_preset].update();
            _changed = false;
        }
    }
    
}