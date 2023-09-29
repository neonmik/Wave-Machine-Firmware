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

        enum Page {
            MAIN = 0,
            FILT = 1,
            LFO  = 2,
            ARP  = 3,

            ADSR = 4,
            fENV = 5,
            SHFT = 6,
            sARP = 7,
        };
           
    }
    class CONTROL {
        private:
            class PAGE {
                private:
                    // volatile bool isActive = true;
                    uint16_t    knobValue[4];
                    bool        isChanged[4];
                    void        (*knobFunctions[4])(uint16_t);
                    bool        buttonState = true;
                    void        (*buttonFunction)(bool); // Optional function pointer to toggle the state
                public:
                    PAGE(void (*function1)(uint16_t), void (*function2)(uint16_t), void (*function3)(uint16_t), void (*function4)(uint16_t), void (*functionB)(bool) = nullptr) {
                        knobFunctions[0] = function1;
                        knobFunctions[1] = function2;
                        knobFunctions[2] = function3;
                        knobFunctions[3] = function4;
                        buttonFunction = functionB;
                        if (buttonFunction != nullptr) buttonState = false;
                        // if (buttonFunction != nullptr) isActive = false;
                    }
                    ~PAGE() { }
                    // void on() {
                    //     setState(true);
                    // }
                    // void off() {
                    //     setState(false);
                    // }
                    // void toggle() {
                    //     setState(!isActive);
                    // }
                    // void setState(bool state) {
                    //     if (buttonFunction != nullptr) {
                    //         isActive = state;
                    //         buttonFunction(isActive);
                    //     }
                    // }
                    // bool getState (void) {
                    //     return isActive;
                    // }
                    void setKnob (uint8_t control, uint16_t value) {
                        knobValue[control] = value;
                        isChanged[control] = true;
                    }
                    uint16_t getKnob (uint8_t control) {
                        return knobValue[control];
                    }
                    void setButton (bool state) {
                        if (buttonFunction != nullptr) {
                            buttonState = state;
                            buttonFunction(buttonState);
                        }
                    }
                    void toggleButton (void) {
                        setButton(!buttonState);
                    }
                    bool getButton (void) {
                        return buttonState;
                    }
                    void update() {
                        // if (isActive) {
                            for (int i = 0; i < 4; i++) {
                                if (isChanged[i]) {
                                    if (knobFunctions[i] != nullptr) knobFunctions[i](knobValue[i]);
                                    isChanged[i] = false;
                                }
                            }
                        // }
                    }
            };

        public:
            CONTROL () { }
            ~CONTROL () { }

            PAGE        MAIN    {&SYNTH::setWaveshape,    &SYNTH::setWavevector,     &SYNTH::setOctave,         &SYNTH::setPitchBend,           nullptr};
                PAGE    ADSR    {&SYNTH::setAttack,       &SYNTH::setDecay,          &SYNTH::setSustain,        &SYNTH::setRelease,             nullptr};
            
            PAGE        FILT    {&FILTER::setCutoff,      &FILTER::setResonance,     &FILTER::setPunch,         &FILTER::setType,               nullptr};
                PAGE    fENV    {&FILTER::setAttack,      &FILTER::setDecay,         &FILTER::setSustain,       &FILTER::setRelease,            nullptr};
            
            PAGE        LFO     {&MOD::setMatrix,         &MOD::setRate,             &MOD::setDepth,            &MOD::setShape,                 MOD::setState};
                PAGE    SHFT    {nullptr,                 nullptr,                   &FX::SOFTCLIP::setGain,    nullptr,                        nullptr};
            
            PAGE        ARP     {&ARP::setGate,           &ARP::setDivision,         &ARP::setRange,            &ARP::setDirection,             ARP::setState};
                PAGE    sARP    {nullptr,                 &ARP::setBPM,              nullptr,                   &ARP::setOctMode,               nullptr};
            

            void init (void) { }

            void setKnob (uint8_t page, uint8_t control, uint16_t value) {
                switch (page) {
                    case Page::MAIN:
                        MAIN.setKnob(control, value);
                        break;
                    case Page::ADSR:
                        ADSR.setKnob(control, value);
                        break;
                    case Page::LFO:
                        LFO.setKnob(control, value);
                        break;
                    case Page::ARP:
                        ARP.setKnob(control, value);
                        break;
                    case Page::SHFT:
                        SHFT.setKnob(control, value);
                        break;
                    case Page::fENV:
                        fENV.setKnob(control, value);
                        break;
                    case Page::FILT:
                        FILT.setKnob(control, value);
                        break;
                    case Page::sARP:
                        sARP.setKnob(control, value);
                        break;
                }
            }
            uint16_t getKnob (uint8_t page, uint16_t control) {
                switch (page) {
                    case Page::MAIN:
                        return MAIN.getKnob(control);
                    case Page::ADSR:
                        return ADSR.getKnob(control);
                    case Page::LFO:
                        return LFO.getKnob(control);
                    case Page::ARP:
                        return ARP.getKnob(control);
                    case Page::SHFT:
                        return SHFT.getKnob(control);
                    case Page::fENV:
                        return fENV.getKnob(control);
                    case Page::FILT:
                        return FILT.getKnob(control);
                    case Page::sARP:
                        return sARP.getKnob(control);
                    default:
                        return 0;
                }
            }
            
            void setButton (uint8_t page, bool state) {
                switch (page) {
                    case Page::MAIN:
                        MAIN.setButton(state);
                        break;
                    case Page::ADSR:
                        ADSR.setButton(state);
                        break;
                    case Page::LFO:
                        LFO.setButton(state);
                        break;
                    case Page::ARP:
                        ARP.setButton(state);
                        break;
                    case Page::SHFT:
                        SHFT.setButton(state);
                        break;
                    case Page::fENV:
                        fENV.setButton(state);
                        break;
                    case Page::FILT:
                        FILT.setButton(state);
                        break;
                    case Page::sARP:
                        sARP.setButton(state);
                        break;
                }
            }
            void toggleButton (uint8_t page) {
                switch (page) {
                    case Page::MAIN:
                        MAIN.toggleButton();
                        break;
                    case Page::ADSR:
                        ADSR.toggleButton();
                        break;
                    case Page::LFO:
                        LFO.toggleButton();
                        break;
                    case Page::ARP:
                        ARP.toggleButton();
                        break;
                    case Page::SHFT:
                        SHFT.toggleButton();
                        break;
                    case Page::fENV:
                        fENV.toggleButton();
                        break;
                    case Page::FILT:
                        FILT.toggleButton();
                        break;
                    case Page::sARP:
                        sARP.toggleButton();
                        break;
                }
            }
            bool getButton (uint8_t page) {
                switch (page) {
                    case Page::MAIN:
                        return MAIN.getButton();
                    case Page::ADSR:
                        return ADSR.getButton();
                    case Page::LFO:
                        return LFO.getButton();
                    case Page::ARP:
                        return ARP.getButton();
                    case Page::SHFT:
                        return SHFT.getButton();
                    case Page::fENV:
                        return fENV.getButton();
                    case Page::FILT:
                        return FILT.getButton();
                    case Page::sARP:
                        return sARP.getButton();
                    default:
                        return false;
                }
            }
            void toggleLFO (void) {
                LFO.toggleButton();
            }
            void set_lfo (bool state) {
                LFO.setButton(state);
            }
            bool getLFO (void) {
                return LFO.getButton();
            }
            
            void toggleArp (void) {
                ARP.toggleButton();
            }
            void set_arp (bool state) {
                ARP.setButton(state);
            }
            bool getArp (void) {
                return ARP.getButton();;
            }
            void update (void) {
                MAIN.update();
                ADSR.update();
                LFO.update();
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
    
    void setKnob (uint8_t page, uint8_t control, uint16_t value);
    uint16_t getKnob (uint8_t page, uint8_t control);
    
    void toggleLFO (void);
    bool getLFO (void);

    void toggleArp (void);
    bool getArp (void);

    void toggleShift (void);
    bool getShift (void);


    void update (void);
};