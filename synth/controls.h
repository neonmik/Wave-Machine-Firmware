#pragma once

#include "string.h"
#include "pico/stdlib.h"
// #include "hardware/sync.h"

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
            ADSR = 1,
            MOD  = 2,
            ARP  = 3,
            SHFT = 4,
            fENV = 5,
            FILT = 6,
            sARP = 7,
        };
           
    }
    class CONTROL {
        private:
            class Page {
                private:
                    volatile bool _active = true;
                    // volatile bool _changed = false;
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
                            // _changed = true;
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
                    void update() {
                        // if (_active && _changed) {
                        if (_active) {
                            for (int i = 0; i < 4; i++) {
                                if (_changed[i]) {
                                    if (_update_funcs[i] != nullptr) _update_funcs[i](_input[i]);
                                    _changed[i] = false;
                                }
                            }
                        }
                        //     for (int i = 0; i < 4; i++) {
                        //         if (_update_funcs[i] != nullptr) _update_funcs[i](_input[i]);
                        //     }
                        // }
                    }
            };

        public:
            CONTROL () { }
            ~CONTROL () { }

            Page        MAIN    {&SYNTH::set_waveshape,    &SYNTH::set_wavevector,     &SYNTH::set_octave,         &SYNTH::set_pitch_scale,         nullptr};
                Page    SHFT    {nullptr,                  nullptr,                    &FX::SOFTCLIP::set_gain,    nullptr,                         nullptr};
            
            Page        ADSR    {&SYNTH::set_attack,       &SYNTH::set_decay,          &SYNTH::set_sustain,        &SYNTH::set_release,             nullptr};
                Page    fENV    {&FILTER::set_attack,      &FILTER::set_decay,         &FILTER::set_sustain,       &FILTER::set_release,            nullptr};
            
            Page        MOD1    {&MOD::set_matrix,         &MOD::set_rate,             &MOD::set_depth,            &MOD::set_shape,                 MOD::set_state};
                Page    FILT    {&FILTER::set_cutoff,      &FILTER::set_resonance,     &FILTER::set_punch,         &FILTER::set_mode,               nullptr};
            
            Page        ARP     {&ARP::set_hold,           &ARP::set_division,         &ARP::set_range,            &ARP::set_direction,             ARP::set_state};
                Page    sARP    {&ARP::set_gap,            &ARP::set_bpm,              nullptr,                    nullptr,                         nullptr};

            //&ARP::set_gap
            
            // Ideas for changing controls layout.
            // Page: 1 - 
            //           OSC    Knob 1: Waveshape,      Knob 2: Wavevector,     Knob 3: Octave,     Knob 4: Pitchbend,      Button: ?
            //           ENV    Knob 1: Attack,         Knob 2: Decay,          Knob 3: Sustain,    Knob 4: Release,        Button: Envelope Bypass 
            // Page: 2 - 
            //           LFO    Knob 1: Waveshape,      Knob 2: Rate,           Knob 3: Depth,      Knob 4: Destination,    Button: LFO BYPASS
            //           ENV    Knob 1: Attack,         Knob 2: Decay,          Knob 3: Sustain,    Knob 4: Release,        Button: Envelope Bypass 
            // Page: 3 - 
            //           FLT    Knob 1: Cutoff,         Knob 2: Resonance,      Knob 3: Punch,      Knob 4: Mode,           Button: FLT BYPASS
            //           ENV    Knob 1: Attack,         Knob 2: Decay,          Knob 3: Sustain,    Knob 4: Release,        Button: Envelope Bypass 
            // Page: 4(ALL) - 
            //           ARP    Knob 1: Hold,           Knob 2: Rate,           Knob 3: Range,      Knob 4: Direction,      Button: ?
            //           ???    Knob 1: Gap,            Knob 2: BPM,            Knob 3: fMONO/PARA, Knob 4: unkown,         Button: unkown 

            

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
                    case 4:
                        SHFT.set(control, input);
                        break;
                    case 5:
                        fENV.set(control, input);
                        break;
                    case 6:
                        FILT.set(control, input);
                        break;
                    case 7:
                        sARP.set(control, input);
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
                    case 4:
                        return SHFT.get(control);
                    case 5:
                        return fENV.get(control);
                    case 6:
                        return FILT.get(control);
                    case 7:
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
            void update (void) {
                MAIN.update();
                ADSR.update();
                MOD1.update();
                ARP.update();
                FILT.update();
                fENV.update();
                SHFT.update();
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

    void toggle_shift (void);
    bool get_shift (void);


    void update (void);
};