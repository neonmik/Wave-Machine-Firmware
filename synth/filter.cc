#include "filter.h"

namespace FILTER {

    ADSREnvelope ADSR{_attack, _decay, _sustain, _release};

    void Init() {
        _lowpass = 0;
        _bandpass = 0;
        _cutoff = 0;
        _resonance = 0;
        _dirty = true;
        _punch = 0;
        _type = LowPass;
    }

    void set_cutoff(uint16_t cutoff) {
        _cutoff = exp_freq(cutoff);
        _dirty = true;
    }

    void set_resonance(uint16_t resonance) {
        _resonance = filter_damp(resonance);
        _dirty = true;
    }

    void modulate_cutoff(uint16_t cutoff) {
        _mod = (cutoff >> 2);
    }

    void set_punch(uint16_t punch) {
        _punch = punch >> 2;
    }

    void set_type(uint16_t type) {
        uint8_t index = (type>>8);
        switch (index) {
        case 0:
            _type = Type::Off;
            break;
        case 1:
            _type = Type::LowPass;
            _direction = Direction::Regular;
            break;
        case 2:
            _type = Type::BandPass;
            _direction = Direction::Regular;
            break;
        case 3:
            _type = Type::HighPass;
            _direction = Direction::Inverted;
            break;
        default:
            break;
        }
    }

    void set_attack (uint16_t attack) {
        if (attack == _last_attack) return;
        _last_attack = attack;
        _attack = calc_end_frame((attack<<2)+2);
    }
    void set_decay (uint16_t decay) {
        if (decay == _last_decay) return;
        _last_decay = decay;
        _decay = calc_end_frame((decay<<2)+2);
    }
    void set_sustain (uint16_t sustain) {
        if (sustain == _last_sustain) return;
        _last_sustain = sustain;
        _sustain = (sustain << 6);
    }
    void set_release (uint16_t release) {
        if (release == _last_release) return;
        _last_release = release;
        _release = calc_end_frame((release<<2)+2);
    }

    void voicesIncrease (void) {
        voices_inc();
    }
    void voicesDecrease (void) {
        voices_dec();
    }

    void trigger_attack (void) {
        if (!voices_active()) return;
        switch (_mode) {
            case Mode::MONO:
                if (!_filter_active) { 
                    ADSR.trigger_attack();
                    _filter_active = true;
                }
                break;
            case Mode::PARA:
                ADSR.trigger_attack();
                _filter_active = true;
                break;
        } 
        
    }
    void trigger_release (void) {
        if (_filter_active && !voices_active()) {
            ADSR.trigger_release();
            _filter_active = false;
        }
    }

    void process(int32_t &sample) {
        if (_type != Type::Off) {
            ADSR.Update();
            
            // dirty is for taking a simple input number and using a lookup table to calculate a smooth frequency input.
            if (_dirty) {
                _frequency = _cutoff;
                _damp = _resonance;
                _dirty = false;
            }


            volatile int32_t frequency = 0 ;
            
            if (_direction == Direction::Regular) {
                frequency = (_frequency * ADSR.get()) >> 16;
            } 
            if (_direction == Direction::Inverted) {
                uint16_t MAX_FREQ = (SAMPLE_RATE/2);
                frequency = MAX_FREQ - (((MAX_FREQ - _frequency) * (ADSR.get())) >> 16);
            }
            
            if (frequency <= 15) frequency = 15;

            int32_t damp = _damp;
            if (_punch) {
                int32_t punch_signal = _lowpass > 4096 ? _lowpass : 2048;
                frequency += ((punch_signal >> 4) * _punch) >> 9;
                damp += ((punch_signal - 2048) >> 3);
            }

            int32_t notch = sample - (_bandpass * damp >> 15);
            _lowpass += frequency * _bandpass >> 15;
            FX::HARDCLIP::process(_lowpass);
            
            int32_t highpass = notch - _lowpass;
            _bandpass += frequency * highpass >> 15;
            FX::HARDCLIP::process(_bandpass);

            switch (_type) {
                case LowPass:
                    sample = _lowpass;
                    break;
                case BandPass:
                    sample = _bandpass;
                    break;
                case HighPass:
                    sample = highpass;
                    break;
                default:
                    sample = sample;
                    break;
            }
        }
    }
}
