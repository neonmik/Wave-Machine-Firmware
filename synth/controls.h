#pragma once

#include "string.h"
#include "pico/stdlib.h"

#include "preset.h"

#include "synth.h"
#include "filter.h"
#include "modulation.h"
#include "arp.h"

#include "../drivers/eeprom.h"

#define MAX_PRESETS     8

namespace CONTROLS {
    namespace {
        uint8_t _preset;
        uint8_t _page;
        bool    _shift;
        bool    _changed;
        uint8_t _poll;

        uint8_t _default_preset = 0;

        enum Controls {
            MAIN = 0,
            FILT = 1,
            MOD  = 2,
            ARP  = 3,

            ADSR = 4,
            fENV = 5,
            SHFT = 6,
            sARP = 7,
        };
           
    }
    class CONTROL {
        private:
            class Page {
                private:
                    volatile bool _active = true;
                    uint16_t _input[4];
                    bool     _changed[4];
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
                            _toggle_func(_active);
                        }
                    }
                    bool get_state() {
                        return _active;
                    }
                    void set(uint8_t control, uint16_t input) {
                        _input[control] = input;
                        _changed[control] = true;
                    }
                    uint16_t get(uint8_t control) {
                        return _input[control];
                    }
                    void Update() {
                        if (_active) {
                            for (int i = 0; i < 4; i++) {
                                if (_changed[i]) {
                                    if (_update_funcs[i] != nullptr) _update_funcs[i](_input[i]);
                                    _changed[i] = false;
                                }
                            }
                        }
                    }
            };

        public:
            CONTROL () { }
            ~CONTROL () { }

            Page        MAIN    {&SYNTH::set_waveshape,    &SYNTH::set_wavevector,     &SYNTH::set_octave,         &SYNTH::set_pitch_scale,         nullptr};
                Page    ADSR    {&SYNTH::set_attack,       &SYNTH::set_decay,          &SYNTH::set_sustain,        &SYNTH::set_release,             nullptr};
            
            Page        FILT    {&FILTER::set_cutoff,      &FILTER::set_resonance,     &FILTER::set_punch,         &FILTER::set_type,               nullptr};
                Page    fENV    {&FILTER::set_attack,      &FILTER::set_decay,         &FILTER::set_sustain,       &FILTER::set_release,            nullptr};
            
            Page        MOD1    {&MOD::set_matrix,         &MOD::set_rate,             &MOD::set_depth,            &MOD::set_shape,                 MOD::set_state};
                Page    SHFT    {nullptr,                  nullptr,                    &FX::SOFTCLIP::set_gain,    nullptr,                         nullptr};
            
            Page        ARP     {&ARP::setGate,            &ARP::setDivision,          &ARP::setRange,             &ARP::setDirection,              ARP::set_state};
                Page    sARP    {nullptr,                  &ARP::setBpm,               nullptr,                    &ARP::setOctMode,                nullptr};
            

            void Init (void) { }

            void set (uint8_t page, uint8_t control, uint16_t input) {
            switch (page) {
                    case Controls::MAIN:
                        MAIN.set(control, input);
                        break;
                    case Controls::ADSR:
                        ADSR.set(control, input);
                        break;
                    case Controls::MOD:
                        MOD1.set(control, input);
                        break;
                    case Controls::ARP:
                        ARP.set(control, input);
                        break;
                    case Controls::SHFT:
                        SHFT.set(control, input);
                        break;
                    case Controls::fENV:
                        fENV.set(control, input);
                        break;
                    case Controls::FILT:
                        FILT.set(control, input);
                        break;
                    case Controls::sARP:
                        sARP.set(control, input);
                        break;
                }
            }
            uint16_t get (uint8_t page, uint16_t control) {
                switch (page) {
                    case Controls::MAIN:
                        return MAIN.get(control);
                    case Controls::ADSR:
                        return ADSR.get(control);
                    case Controls::MOD:
                        return MOD1.get(control);
                    case Controls::ARP:
                        return ARP.get(control);
                    case Controls::SHFT:
                        return SHFT.get(control);
                    case Controls::fENV:
                        return fENV.get(control);
                    case Controls::FILT:
                        return FILT.get(control);
                    case Controls::sARP:
                        return sARP.get(control);
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
                return MOD1.get_state();
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
            void Update (void) {
                MAIN.Update();
                ADSR.Update();
                MOD1.Update();
                ARP.Update();
                SHFT.Update();
                fENV.Update();
                FILT.Update();
                sARP.Update();
            }
    };
    

    extern CONTROL Control;
    extern PRESET Preset[MAX_PRESETS];
    
    void Init (void);
    
    void set_preset (uint8_t preset);
    uint8_t get_preset (void);
    void save_preset (uint8_t preset);
    void load_preset (uint8_t preset);
    void export_presets (void);
    void factory_restore (void);
    void write_factory_presets (void);

    void save (void);

    void set_page (uint8_t page);
    uint8_t get_page (void);
    
    void set_value (uint8_t page, uint8_t control, uint16_t input);
    uint16_t get_value (uint8_t page, uint8_t control);
    
    void toggle_lfo (void);
    bool get_lfo (void);

    void toggle_arp (void);
    bool get_arp (void);

    void toggle_shift (void);
    bool get_shift (void);


    void Update (void);
};