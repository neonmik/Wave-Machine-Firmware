#pragma once

#include "string.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

#include "preset.h"

#include "synth.h"
#include "modulation.h"
#include "arp.h"

#include "../drivers/eeprom.h"
// #include "../mailbox.h"

#define MAX_PRESETS     8

namespace SETTINGS {
    namespace {
        uint8_t _preset;
        uint8_t _page;
        bool    _changed;

        uint8_t _default_preset = 0;

        
    }
    class CONTROL {
        private:
            class Page {
                private:
                    volatile bool _active = true;
                    volatile bool _changed = false;
                    uint16_t _input[4];
                    void (*_update_funcs[4])(uint16_t);
                    void (*_toggle_func)(bool); // Optional function pointer to toggle the state
                public:
                    Page(void (*func1)(uint16_t), void (*func2)(uint16_t), void (*func3)(uint16_t), void (*func4)(uint16_t), void (*toggle_func)(bool) = nullptr) {
                        _update_funcs[0] = func1;
                        _update_funcs[1] = func2;
                        _update_funcs[2] = func3;
                        _update_funcs[3] = func4;
                        _toggle_func = toggle_func;
                        if (_toggle_func != nullptr) _active = false;
                    }
                    ~Page() { }
                    void on() {
                        set_state(true);
                    }
                    void off() {
                        set_state(false);
                    }
                    void toggle() {
                        set_state(!_active);
                    }
                    void set_state(bool state) {
                        if (_toggle_func != nullptr) {
                            _active = state;
                            _changed = true;
                            _toggle_func(_active);
                        }
                    }
                    bool get_state() {
                        return _active;
                    }
                    void set(uint8_t control, uint16_t input) {
                        _input[control] = input;
                        _changed = true;
                    }
                    uint16_t get(uint8_t control) {
                        return _input[control];
                    }
                    void update() {
                        if (_active && _changed) {
                            for (int i = 0; i < 4; i++) {
                                _update_funcs[i](_input[i]);
                            }
                            _changed = false;
                        }
                    }
            };

        public:
            CONTROL () { }
            ~CONTROL () { }

            Page    MAIN {&SYNTH::set_waveshape,    &SYNTH::set_wavevector,     &SYNTH::set_octave,         &SYNTH::set_pitch_scale,        nullptr};
            Page    ADSR {&SYNTH::set_attack,       &SYNTH::set_decay,          &SYNTH::set_sustain,        &SYNTH::set_release,            nullptr};
            Page    MOD1 {&MOD::set_matrix,         &MOD::set_rate,             &MOD::set_depth,            &MOD::set_shape,                 MOD::set_state};
            Page    ARP  {&ARP::set_hold,           &ARP::set_division,         &ARP::set_range,            &ARP::set_direction,            ARP::set_state};

            void init (void) { }

            void set (uint8_t page, uint8_t control, uint16_t input) {
            switch (page) {
                    case 0:
                        MAIN.set(control, input);
                        break;
                    case 1:
                        ADSR.set(control, input);
                        break;
                    case 2:
                        MOD1.set(control, input);
                        break;
                    case 3:
                        ARP.set(control, input);
                        break;
                }
            }
            uint16_t get (uint8_t page, uint16_t control) {
                uint16_t temp;
                switch (page) {
                    case 0:
                        return MAIN.get(control);
                    case 1:
                        return ADSR.get(control);
                    case 2:
                        return MOD1.get(control);
                    case 3:
                        return ARP.get(control);
                    default:
                        return 0;
                }
            }
            
            void toggle_lfo (void) {
                MOD1.toggle();
            }
            void set_lfo (bool state) {
                MOD1.set_state(state);
            }
            bool get_lfo (void) {
                return MOD1.get_state();;
            }
            
            void toggle_arp (void) {
                ARP.toggle();
            }
            void set_arp (bool state) {
                ARP.set_state(state);
            }
            bool get_arp (void) {
                return ARP.get_state();;
            }
            void update (void) {
                MAIN.update();
                ADSR.update();
                MOD1.update();
                ARP.update();
            }
    };
    

    extern CONTROL Control;
    extern PRESET Preset[MAX_PRESETS];
    
    void init (void);
    
    void set_preset (uint8_t preset);
    uint8_t get_preset (void);
    void save_preset (uint8_t preset);
    void load_preset (uint8_t preset);
    void export_presets (void);
    void factory_restore (void);

    void save (void);

    void set_page (uint8_t page);
    uint8_t get_page (void);
    
    void set_value (uint8_t page, uint8_t control, uint16_t input);
    uint16_t get_value (uint8_t page, uint8_t control);
    
    void toggle_lfo (void);
    bool get_lfo (void);

    void toggle_arp (void);
    bool get_arp (void);

    void update (void);
};