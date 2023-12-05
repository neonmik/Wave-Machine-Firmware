#pragma once

#include "config.h"

#include "pagination.h"

#include "drivers/eeprom.h"
#include "drivers/leds.h"

#include "synth/preset.h"

#include "synth/synth.h"
#include "synth/filter.h"
#include "synth/modulation.h"
#include "synth/arp.h"

namespace CONTROLS
{
    namespace
    {
        uint16_t    SHIFT_TIMEOUT           = 75;

        uint8_t     currentPreset;
        uint8_t     currentPage;
        uint8_t     activePage;
        uint8_t     extraPage;

        bool shift;
        uint16_t shiftCounter;

        bool inExtraPages;

        bool needsUpdating;

        enum Page
        {
            MAIN = 0,
            FILT = 1,
            LFO = 2,
            ARP = 3,

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
                    uint16_t knobValue[4];
                    
                    bool isChanged[4];

                    void (*knobFunctions[4])(uint16_t);

                    bool buttonState[2];
                    void (*buttonFunction[2])(bool); // Optional function pointer for toggle button
                public:
                    PAGE(   void (*knobFunction1)(uint16_t), void (*knobFunction2)(uint16_t), void (*knobFunction3)(uint16_t), void (*knobFunction4)(uint16_t), 
                            void (*buttonFunction1)(bool) = nullptr, void (*buttonFunction2)(bool) = nullptr)
                    {
                        knobFunctions[0] = knobFunction1;
                        knobFunctions[1] = knobFunction2;
                        knobFunctions[2] = knobFunction3;
                        knobFunctions[3] = knobFunction4;
                        buttonFunction[0] = buttonFunction1;
                        buttonFunction[1] = buttonFunction2;

                        // This is a throwback to old function handling - will be removed when control API is updated.
                        buttonState[0] = true;
                        buttonState[1] = true;

                        if (buttonFunction[0] != nullptr)
                            buttonState[0] = false;
                        if (buttonFunction[1] != nullptr)
                            buttonState[1] = false;
                            
                    }
                    ~PAGE() {}
                    void setKnob(uint8_t control, uint16_t value) {
                        knobValue[control] = value;
                        isChanged[control] = true;
                    }
                    uint16_t getKnob(uint8_t control) {
                        return knobValue[control];
                    }
                    void setButton(uint8_t button, bool state) {
                        if (buttonFunction[button] != nullptr) {
                            buttonState[button] = state;
                            buttonFunction[button](buttonState[button]);
                        }
                    }
                    void toggleButton(uint8_t button) {
                        setButton(button, !buttonState[button]);
                    }
                    bool getButton(uint8_t button) {
                        if (buttonFunction[button] != nullptr) return buttonState[button];
                        else return false;
                    }
                    void update(void) {
                        for (int i = 0; i < 4; i++) {
                            if (isChanged[i]) {
                                if (knobFunctions[i] != nullptr)
                                    knobFunctions[i](knobValue[i]);
                                isChanged[i] = false;
                            }
                        }
                    }
                };

        public:
            CONTROL( ) { }
            ~CONTROL( ) { }

            //          PAGE  | KNOB 1                   | KNOB 2                    | KNOB 3                    | KNOB 4                    | BUTTON 1                    | BUTTON 2 |
            PAGE        MAIN{   &SYNTH::setWaveShape,      &SYNTH::setWaveVector,      &SYNTH::setOctave,          &SYNTH::setPitchBend,       nullptr,                      nullptr};
                PAGE    ADSR{   &SYNTH::setAttack,         &SYNTH::setDecay,           &SYNTH::setSustain,         &SYNTH::setRelease,         nullptr,                      nullptr};

            PAGE        FILT{   &FILTER::setCutoff,        &FILTER::setResonance,      &FILTER::setPunch,          &FILTER::setType,           FILTER::setState,             nullptr};
                PAGE    fENV{   &FILTER::setAttack,        &FILTER::setDecay,          &FILTER::setSustain,        &FILTER::setRelease,        nullptr,                      nullptr};

            PAGE        LFO {   &MOD::setMatrix,           &MOD::setRate,              &MOD::setDepth,             &MOD::setShape,             MOD::setState,                nullptr};
                PAGE    SHFT{   &SYNTH::setOsc2Wave,       &SYNTH::setNoise,           &FX::SOFTCLIP::setGain,     &SYNTH::setDetune,          nullptr,                      nullptr};

            PAGE        ARP {   &ARP::setGate,             &ARP::setDivision,          &ARP::setRange,             &ARP::setDirection,         ARP::setState,                nullptr};
                PAGE    sARP{   nullptr,                   &ARP::setBPM,               &FILTER::setTriggerMode,    &ARP::setOctMode,           nullptr,                      nullptr};

            // Currently new/unused pages
                PAGE    mENV{   &MOD::setAttack,           &MOD::setDecay,             &MOD::setSustain,           &MOD::setRelease,           nullptr,                      nullptr};
                

            void init(void) {}

            void setKnob(uint8_t page, uint8_t control, uint16_t value) {
                switch (page) {
                    case Page::MAIN:            MAIN.setKnob(control, value);   break;
                    case Page::FILT:            FILT.setKnob(control, value);   break;
                    case Page::LFO:             LFO.setKnob(control, value);    break;
                    case Page::ARP:             ARP.setKnob(control, value);    break;
                    case Page::ADSR:            ADSR.setKnob(control, value);   break;
                    case Page::fENV:            fENV.setKnob(control, value);   break;
                    case Page::SHFT:            SHFT.setKnob(control, value);   break;
                    case Page::sARP:            sARP.setKnob(control, value);   break;
                }
            }
            uint16_t getKnob(uint8_t page, uint16_t control) {
                switch (page) {
                    case Page::MAIN:            return                          MAIN.getKnob(control);
                    case Page::FILT:            return                          FILT.getKnob(control);
                    case Page::LFO:             return                          LFO.getKnob(control);
                    case Page::ARP:             return                          ARP.getKnob(control);
                    case Page::ADSR:            return                          ADSR.getKnob(control);
                    case Page::fENV:            return                          fENV.getKnob(control);
                    case Page::SHFT:            return                          SHFT.getKnob(control);
                    case Page::sARP:            return                          sARP.getKnob(control);
                    default:                    return                          0;
                }
            }

            void setButton(uint8_t page, uint8_t button, bool state) {
                switch (page) {
                    case Page::MAIN:            MAIN.setButton(button, state);          break;
                    case Page::FILT:            FILT.setButton(button, state);          break;
                    case Page::LFO:             LFO.setButton(button, state);           break;
                    case Page::ARP:             ARP.setButton(button, state);           break;
                    case Page::ADSR:            ADSR.setButton(button, state);          break;
                    case Page::fENV:            fENV.setButton(button, state);          break;
                    case Page::SHFT:            SHFT.setButton(button, state);          break;
                    case Page::sARP:            sARP.setButton(button, state);          break;
                }
            }
            void toggleButton(uint8_t page, uint8_t button) {
                switch (page)   {
                    case Page::MAIN:            MAIN.toggleButton(button);            break;
                    case Page::ADSR:            ADSR.toggleButton(button);            break;
                    case Page::LFO:             LFO.toggleButton(button);             break;
                    case Page::ARP:             ARP.toggleButton(button);             break;
                    case Page::SHFT:            SHFT.toggleButton(button);            break;
                    case Page::fENV:            fENV.toggleButton(button);            break;
                    case Page::FILT:            FILT.toggleButton(button);            break;
                    case Page::sARP:            sARP.toggleButton(button);            break;
                }
            }
            bool getButton(uint8_t page, uint8_t button) {
                switch (page) {
                    case Page::MAIN:            return                          MAIN.getButton(button);
                    case Page::FILT:            return                          FILT.getButton(button);
                    case Page::LFO:             return                          LFO.getButton(button);
                    case Page::ARP:             return                          ARP.getButton(button);
                    case Page::ADSR:            return                          ADSR.getButton(button);
                    case Page::fENV:            return                          fENV.getButton(button);
                    case Page::SHFT:            return                          SHFT.getButton(button);
                    case Page::sARP:            return                          sARP.getButton(button);
                    default:                    return                          false;
                }
            }

            void update(uint8_t index) {
                switch (index) {
                    case Page::MAIN:            MAIN.update();                  break;
                    case Page::FILT:            FILT.update();                  break;
                    case Page::LFO:             LFO.update();                   break;
                    case Page::ARP:             ARP.update();                   break;
                    case Page::ADSR:            ADSR.update();                  break;
                    case Page::fENV:            fENV.update();                  break;
                    case Page::SHFT:            SHFT.update();                  break;
                    case Page::sARP:            sARP.update();                  break;
                }
            }
            void updateAll(void) {
                MAIN.update();
                FILT.update();
                LFO.update();
                ARP.update();
                ADSR.update();
                fENV.update();
                SHFT.update();
                sARP.update();
            }
    };

    extern CONTROL Control;
    extern PRESET Preset[MAX_PRESETS];

    void init(void);

    void setButtonAssignment (void);

    void setPreset(uint8_t preset);
    void changePreset (void);
    uint8_t getPreset(void);

    void savePresetToSlot(uint8_t slot);
    void loadPresetFromSlot(uint8_t slot);
    void exportPresets(void);
    void factoryRestore(void);
    void updateFactoryPresets(void);

    void save(void);

    void setPage(uint8_t page);
    void changePage(void);
    uint8_t getPage(void);

    void setKnob (uint8_t page, uint8_t control, uint16_t input);
    uint16_t getKnob(uint8_t page, uint8_t control);

    void setButton (uint8_t page, uint8_t button, bool state);
    void toggleButton (uint8_t page, uint8_t button);
    bool getButton (uint8_t page, uint8_t button);

    void toggleButton1 (void);
    void holdButton1 (void);
    bool getButton1 (void);

    void toggleButton2 (void);
    void holdButton2 (void);
    bool getButton2 (void);

    void setShift(bool input);
    void resetShift (void);

    void update(void);

    void refreshInterface (void);
};