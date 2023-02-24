#pragma once

#include "pico/stdlib.h"

#include "synth.h"
#include "modulation.h"
#include "arp.h"

#include "log_table.h"

#define MAX_PRESETS     7

namespace SETTINGS {
    namespace {
        uint8_t _preset;
        uint8_t _page;
        bool    _changed;
        uint16_t get_pitch_log (int index) {
            uint16_t pitch = log_table[index];
            return pitch;
        }
    }
    class PRESET {
        private:
            bool _lfo_active;
            bool _arp_active;
            
            class Wave {
                private:
                    uint16_t _input[4];
                    uint16_t _waveshape;
                    uint16_t _wavevector;
                    uint8_t _octave;
                    uint16_t _pitch;
                public:
                Wave () { }
                ~Wave () { }

                void set (uint8_t control, uint16_t input) {
                    _input[control] = input;
                    switch (control) {
                        case 0:
                            _waveshape = ((input>>6)*256);
                            break;
                        case 1:
                            _wavevector = input;
                            break;
                        case 2:
                            _octave = (input>>8);
                            break;
                        case 3:
                            _pitch = get_pitch_log(input);
                            break;
                    }
                }
                uint16_t get (uint8_t control) {
                    return _input[control];
                }
                void update (void) {
                    SYNTH::wave = _waveshape;
                    SYNTH::wave_vector = _wavevector;
                    SYNTH::octave = _octave;
                    SYNTH::pitch_scale = _pitch;
                }
                void fetch (void) {
                    //pull defaults from function, for now
                    _waveshape = SYNTH::wave;
                    _wavevector = SYNTH::wave_vector;
                    _octave = SYNTH::octave;
                    _pitch = SYNTH::pitch_scale;
                }
            };
            class Env {
                private:
                    bool _active = true;
                    uint16_t _input [4];
                    uint16_t _attack;
                    uint16_t _decay;
                    uint16_t _sustain;
                    uint16_t _release;
                public:
                    Env () { }
                    ~Env () { }
                    void on (void) {
                        _active = true;
                    }
                    void off (void) {
                        _active = false;
                    }
                    void toggle (void) {
                        _active = !_active;
                    }
                    void set (uint8_t control, uint16_t input) {
                        _input[control] = input;
                        switch (control) {
                            case 0:
                                _attack = ((input<<2)+10);
                                break;
                            case 1:
                                _decay = ((input<<2)+10);
                                break;
                            case 2:
                                _sustain = (input<<6);
                                break;
                            case 3:
                                _release = (input<<2);
                                break;
                        }
                    }
                    uint16_t get (uint8_t control) {
                        return _input[control];
                    }
                    void update (void) {
                        SYNTH::attack_ms = _attack;
                        SYNTH::decay_ms = _decay;
                        SYNTH::sustain = _sustain;
                        SYNTH::release_ms = _release;
                    }
                    void fetch (void) {
                        //pull defaults from function, for now
                        _attack = SYNTH::attack_ms;
                        _decay = SYNTH::decay_ms;
                        _sustain = SYNTH::sustain;
                        _release = SYNTH::release_ms;
                    }
            };
            class Lfo {
                private:
                    bool _active;
                    uint16_t _input[4];
                    uint16_t _matrix;
                    uint16_t _rate;
                    uint16_t _depth;
                    uint16_t _waveshape;
                public:
                    Lfo () { }
                    ~Lfo () { }
                    void on (void) {
                        _active = true;
                    }
                    void off (void) {
                        _active = false;
                    }
                    void toggle (void) {
                        _active = !_active;
                        if (_active) MOD::MOD1.start();
                        if (!_active) MOD::MOD1.stop();
                    }
                    void set (uint8_t control, uint16_t input) {
                        _input[control] = input;
                        switch (control) {
                            case 0:
                                _matrix = input;
                                break;
                            case 1:
                                _rate = input;
                                break;
                            case 2:
                                _depth = input;
                                break;
                            case 3:
                                _waveshape = input;
                                break;
                        }
                    }
                    uint16_t get (uint8_t control) {
                        return _input[control];
                    }
                    void update (void) { 
                        if (_active) {  
                            MOD::MOD1.set_matrix(_matrix);
                            MOD::MOD1.set_rate(_rate);
                            MOD::MOD1.set_depth(_depth);
                            MOD::MOD1.set_wave(_waveshape);
                        }
                    }
                    void fetch (void) {
                        //pull defaults from function, for now
                        
                    }
            };
            class Arp {
                private:
                    bool _active;
                    bool _toggled;
                    bool _changed;
                    uint16_t _input[4];
                    uint16_t _matrix;
                    uint16_t _rate;
                    uint16_t _depth;
                    uint16_t _direction;
                public:
                    Arp () { }
                    ~Arp () { }

                    void on (void) {
                        _active = true;
                    }
                    void off (void) {
                        _active = false;
                    }
                    void toggle (void) {
                        _active = !_active;
                        // if (_active) ARP::on();
                        // if (!_active) ARP::off();
                        // _toggled = true;
                    }
                    void set (uint8_t control, uint16_t input) {
                        _input[control] = input;
                        _changed = true;
                        switch (control) {
                            case 0:
                                _matrix = input;
                                break;
                            case 1:
                                _rate = (input>>3);
                                break;
                            case 2:
                                _depth = input;
                                break;
                            case 3:
                                _direction = (input>>8);
                                break;
                        }
                    }
                    void update (void) {
                        // if (_toggled) {
                            if (_active) ARP::on();
                            if (!_active) ARP::off();
                        //     _toggled = false;
                        // }
                        if (_active) {
                             if (_changed) {
                                ARP::set_bpm(_rate);
                                ARP::set_direction(_direction);
                                
                                _changed = false;
                            }
                            
                        }
                        
                    }
                    uint16_t get (uint8_t control) {
                        uint16_t temp;
                        switch (control) {
                            case 0:
                                temp = _matrix;
                                break;
                            case 1:
                                temp = _rate;
                                break;
                            case 2:
                                temp = _depth;
                                break;
                            case 3:
                                temp = _direction;;
                                break;
                        }
                        return temp;
                    }
                    void fetch (void) {
                        //pull defaults from function, for now
                        _rate = ARP::get_bpm();
                    }
            };
        public:
            PRESET () { }
            ~PRESET () { }
            
            Wave    MAIN;
            Env     ADSR;
            Lfo     MOD1;
            // Lfo     MOD2;
            // Lfo     MOD3;
            Arp     ARP;

            void init (void) {
            }
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
                        // Shift on MAIN page
                        break;
                    case 5:
                        // shift on ADSR page
                        break;
                    case 6:
                        // Shift on MOD page
                        // MOD2.set(control, input);
                        break;
                    case 7:
                        // Shift on ARP page
                        break;
                }
            }
            uint16_t get (uint8_t page, uint16_t control) {
                uint16_t temp;
                switch (page) {
                    case 0:
                        temp = MAIN.get(control);
                        break;
                    case 1:
                        temp = ADSR.get(control);
                        break;
                    case 2:
                        temp = MOD1.get(control);
                        break;
                    case 3:
                        temp = ARP.get(control);
                        break;
                    case 4:
                        // Shift on MAIN page
                        break;
                    case 5:
                        // shift on ADSR page
                        break;
                    case 6:
                        // Shift on MOD page
                        // temp = MOD2.get(control);
                        break;
                    case 7:
                        // Shift on ARP page
                        break;
                }
                return temp;
            }
            
            void toggle_lfo (void) {
                _lfo_active = !_lfo_active;
                MOD1.toggle();
                // MOD2.toggle();
            }
            bool get_lfo (void) {
                return _lfo_active;
            }
            
            void toggle_arp (void) {
                _arp_active = !_arp_active;
                ARP.toggle();
            }
            bool get_arp (void) {
                return _arp_active;
            }
            
            void fetch (void) {
                MAIN.fetch();
                ADSR.fetch();
                MOD1.fetch();
                ARP.fetch();
            }
            void update (void) {
                MAIN.update();
                ADSR.update();
                MOD1.update();
                ARP.update();
            }
    };
    
    extern PRESET Preset[8];
    
    void init (void);
    
    void set_preset (uint8_t preset);
    uint8_t get_preset (void);

    void set_page (uint8_t page);
    uint8_t get_page (void);
    
    void set_value (uint8_t page, uint8_t control, uint16_t input);
    uint16_t get_value (uint8_t page, uint8_t control);
    
    void toggle_lfo (void);
    bool get_lfo (void);


    void toggle_arp (void);
    bool get_arp (void);


    void fetch ();
    void update (void);
};