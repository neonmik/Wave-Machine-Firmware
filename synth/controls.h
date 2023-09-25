#pragma once

#include "string.h"
#include "pico/stdlib.h"

#include "preset.h"

#include "synth.h"
#include "filter.h"
#include "modulation.h"
#include "arp.h"

#include "../drivers/eeprom.h"

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
                        setState(true);
                    }
                    void off() {
                        setState(false);
                    }
                    void toggle() {
                        setState(!_active);
                    }
                    void setState(bool state) {
                        if (_toggle_func != nullptr) {
                            _active = state;
                            _toggle_func(_active);
                        }
                    }
                    bool getState() {
                        return _active;
                    }
                    void set(uint8_t control, uint16_t input) {
                        _input[control] = input;
                        _changed[control] = true;
                    }
                    uint16_t get(uint8_t control) {
                        return _input[control];
                    }
                    void update() {
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

            Page        MAIN    {&SYNTH::setWaveshape,    &SYNTH::setWavevector,     &SYNTH::setOctave,         &SYNTH::setPitchBend,         nullptr};
                Page    ADSR    {&SYNTH::setAttack,       &SYNTH::setDecay,          &SYNTH::setSustain,        &SYNTH::setRelease,             nullptr};
            
            Page        FILT    {&FILTER::setCutoff,      &FILTER::setResonance,     &FILTER::setPunch,         &FILTER::setType,               nullptr};
                Page    fENV    {&FILTER::setAttack,      &FILTER::setDecay,         &FILTER::setSustain,       &FILTER::setRelease,            nullptr};
            
            Page        MOD1    {&MOD::setMatrix,         &MOD::setRate,             &MOD::setDepth,            &MOD::setShape,                 MOD::setState};
                Page    SHFT    {nullptr,                  nullptr,                    &FX::SOFTCLIP::setGain,    nullptr,                         nullptr};
            
            Page        ARP     {&ARP::setGate,            &ARP::setDivision,          &ARP::setRange,             &ARP::setDirection,              ARP::setState};
                Page    sARP    {nullptr,                  &ARP::setBPM,               nullptr,                    &ARP::setOctMode,                nullptr};
            

            void init (void) { }

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
            
            void toggleLFO (void) {
                MOD1.toggle();
            }
            void set_lfo (bool state) {
                MOD1.setState(state);
            }
            bool getLFO (void) {
                return MOD1.getState();
            }
            
            void toggleArp (void) {
                ARP.toggle();
            }
            void set_arp (bool state) {
                ARP.setState(state);
            }
            bool getArp (void) {
                return ARP.getState();;
            }
            void update (void) {
                MAIN.update();
                ADSR.update();
                MOD1.update();
                ARP.update();
                SHFT.update();
                fENV.update();
                FILT.update();
                sARP.update();
            }
    };
    

    extern CONTROL Control;
    extern PRESET Preset[MAX_PRESETS];
    
    void init (void);
    
    void set_preset (uint8_t preset);
    uint8_t get_preset (void);
    void save_preset (uint8_t preset);
    void load_preset (uint8_t preset);
    void exportPresets (void);
    void factoryRestore (void);
    void updateFactoryPresets (void);

    void save (void);

    void setPage (uint8_t page);
    uint8_t getPage (void);
    
    void setValue (uint8_t page, uint8_t control, uint16_t input);
    uint16_t getValue (uint8_t page, uint8_t control);
    
    void toggleLFO (void);
    bool getLFO (void);

    void toggleArp (void);
    bool getArp (void);

    void toggleShift (void);
    bool getShift (void);


    void update (void);
};