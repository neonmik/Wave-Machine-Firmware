#pragma once

#include "string.h"
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

// #include "synth.h"
// #include "modulation.h"
// #include "arp.h"

#include "../mailbox.h"

#define MAX_PRESETS     8

namespace SETTINGS {
    struct PRESET {
        struct Wav {
            uint16_t shape = 0;
            uint16_t vector = 0;
            uint16_t octave = 0;
            uint16_t pitch = 511;
        };
        struct Env {
            uint16_t attack = 10;
            uint16_t decay = 10;
            uint16_t sustain = 1023;
            uint16_t release = 10;
        };
        struct Lfo {
            bool state = false;
            uint16_t matrix = 0;
            uint16_t rate = 0;
            uint16_t depth = 0;
            uint16_t wave = 0;
        };
        struct Arp {
            bool state = false;
            uint16_t hold = 0;
            uint16_t divisisions = 511;
            uint16_t range = 0;
            uint16_t direction = 0;
        };
        Wav Wave;
        Env Envelope;
        Lfo Modulation;
        Arp Arpeggiator;
    };

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
                    uint16_t* _update_vars[4];
                    bool* _toggle_state; // Optional boolean reference to toggle the state
                public:
                    Page(uint16_t* var1, uint16_t* var2, uint16_t* var3, uint16_t* var4, bool* toggle_state = nullptr) :
                        _update_vars{var1, var2, var3, var4},
                        _toggle_state(toggle_state)
                    {
                        if (_toggle_state != nullptr) _active = false;
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
                        if (_toggle_state != nullptr) {
                            _active = state;
                            *_toggle_state = state;
                            _changed = true;
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
                                *_update_vars[i] = _input[i];
                            }
                            _changed = false;
                        }
                    }
                    void fetch() {
                    }
            };

        public:
            CONTROL () { }
            ~CONTROL () { }
            
            MAILBOX::synth_data& SYNTH_DATA = MAILBOX::SYNTH_DATA.core1;
            MAILBOX::mod_data& MOD_DATA = MAILBOX::MOD_DATA.core1;
            MAILBOX::arp_data& ARP_DATA = MAILBOX::ARP_DATA.core1;

            Page    MAIN {&SYNTH_DATA.waveshape,     &SYNTH_DATA.vector,      &SYNTH_DATA.octave,      &SYNTH_DATA.pitch};
            Page    ADSR {&SYNTH_DATA.attack,        &SYNTH_DATA.decay,       &SYNTH_DATA.sustain,     &SYNTH_DATA.release};
            Page    MOD1 {&MOD_DATA.matrix,          &MOD_DATA.rate,          &MOD_DATA.depth,         &MOD_DATA.shape,              &MOD_DATA.enabled};
            Page    ARP  {&ARP_DATA.hold,            &ARP_DATA.division,      &ARP_DATA.range,         &ARP_DATA.direction,          &ARP_DATA.enabled};

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
            
            // void fetch (void) {
            //     MAIN.fetch();
            //     ADSR.fetch();
            //     MOD1.fetch();
            //     ARP.fetch();
            // }
            void update (void) {
                MAIN.update();
                ADSR.update();
                MOD1.update();
                ARP.update();
                MAILBOX::send();
            }
    };
    

    extern CONTROL Control;
    extern PRESET Preset[MAX_PRESETS];
    
    void init (void);
    
    void set_preset (uint8_t preset);
    uint8_t get_preset (void);
    void save_preset (void);
    void load_preset (void);

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