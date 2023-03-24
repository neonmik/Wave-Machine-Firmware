#pragma once

#include "pico/stdlib.h"

#include "synth.h"
#include "modulation.h"
#include "arp.h"



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
                    uint16_t _octave;
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
                    SYNTH::_wave_shape = _waveshape;
                    SYNTH::_wave_vector =_wavevector;
                    SYNTH::_octave = _octave;
                    SYNTH::_pitch_scale = _pitch;
                }
                void fetch (void) {
                    //pull defaults from function, for now
                    _waveshape = SYNTH::_wave_shape;
                    _wavevector = SYNTH::_wave_vector;
                    _octave = SYNTH::_octave;
                    _pitch = SYNTH::_pitch_scale;
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
                                _release = ((input<<2)+10);
                                break;
                        }
                    }
                    uint16_t get (uint8_t control) {
                        return _input[control];
                    }
                    void update (void) {
                        SYNTH::_attack_ms  = _attack;
                        SYNTH::_decay_ms = _decay;
                        SYNTH::_sustain = _sustain;
                        SYNTH::_release_ms = _release;
                    }
                    void fetch (void) {
                        //pull defaults from function, f
                        _attack = SYNTH::_attack_ms;
                        _input[0] = ((SYNTH::_attack_ms>>2));
                        _decay = SYNTH::_decay_ms;
                        _input[1] = ((SYNTH::_decay_ms>>2));
                        _sustain = SYNTH::_sustain;
                        _input[2] = (SYNTH::_sustain>>6);
                        _release = SYNTH::_release_ms;
                        _input[3] = (SYNTH::_release_ms>>2);
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
                        _toggled = true;
                    }
                    void set (uint8_t control, uint16_t input) {
                        if (input != _input[control]) {
                            _input[control] = input;
                            _changed = true;
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
                                    _direction = input;
                                    break;
                            }
                        }
                    }
                    void update (void) {
                        if (_toggled) {
                            ARP::set(_active);
                            _toggled = false;
                        }
                        if (_active) {
                             if (_changed) {
                                ARP::set_hold(_matrix);
                                ARP::set_bpm(_rate);
                                ARP::set_direction(_direction);
                                
                                _changed = false;
                            }
                            
                        }
                        
                    }
                    uint16_t get (uint8_t control) {
                        return _input[control];
                    }
                    void fetch (void) {
                        // pull defaults from function, for now
                        _input[0] = 0;
                        _input[1] = (ARP::get_bpm());
                        _rate = ARP::get_bpm();
                        _input[2] = 0;
                        _input[3] = 0;
                        _changed = true;
                    }
            };
        public:
            PRESET () { }
            ~PRESET () { }
            
            Wave    MAIN;
            Env     ADSR;
            Lfo     MOD1;
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