#include "filter.h"

namespace FILTER {

    ADSREnvelope ADSR{_attack, _decay, _sustain, _release};

    void init() {
        lowPass = 0;
        bandPass = 0;
        needsUpdating = true;
    }

    void setCutoff(uint16_t cutoff) {
        _cutoff = exponentialFrequency(cutoff);
        needsUpdating = true;
    }

    void setResonance(uint16_t resonance) {
        _resonance = filter_damp(resonance);
        needsUpdating = true;
    }


    void setPunch(uint16_t punch) {
        _punch = punch >> 2;
    }

    void setType(uint16_t type) {
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

    void setDirection (uint16_t direction) {
        uint8_t index = (direction>>9);
        switch (index) {
        case 0:
            _direction = Direction::Regular;
            break;
        case 1:
            _direction = Direction::Inverted;
            break;
        default:
            break;
        }
    }

    void modulateCutoff(uint16_t cutoff) {
        _mod = (cutoff >> 2);
    }

    void setAttack (uint16_t attack) {
        if (attack == lastAttack) return;
        lastAttack = attack;
        _attack = calculateEndFrame(attack << 2);
    }
    void setDecay (uint16_t decay) {
        if (decay == lastDecay) return;
        lastDecay = decay;
        _decay = calculateEndFrame(decay << 2);
    }
    void setSustain (uint16_t sustain) {
        if (sustain == lastSustain) return;
        lastSustain = sustain;
        _sustain = (sustain << 6);
    }
    void setRelease (uint16_t release) {
        if (release == lastRelease) return;
        lastRelease = release;
        _release = calculateEndFrame(release << 2);
    }

    void voicesIncrease (void) {
        voices_inc();
    }
    void voicesDecrease (void) {
        voices_dec();
    }

    void triggerAttack (void) {
        if (!voicesActive()) return;
        switch (_mode) {
            case Mode::MONO:
                if (!filterActive) { 
                    ADSR.triggerAttack();
                    filterActive = true;
                }
                break;
            case Mode::PARA:
                ADSR.triggerAttack();
                filterActive = true;
                break;
        } 
        
    }
    void triggerRelease (void) {
        if (filterActive && !voicesActive()) {
            ADSR.triggerRelease();
            filterActive = false;
        }
    }

    void process(int32_t &sample) {
        if (_type != Type::Off) {
            ADSR.update();
            
            // dirty is for taking a simple input number and using a lookup table to calculate a smooth frequency input.
            if (needsUpdating) {
                _frequency = _cutoff;
                _damp = _resonance;
                needsUpdating = false;
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
                int32_t punch_signal = lowPass > 4096 ? lowPass : 2048;
                frequency += ((punch_signal >> 4) * _punch) >> 9;
                damp += ((punch_signal - 2048) >> 3);
            }

            int32_t notch = sample - (bandPass * damp >> 15);
            lowPass += frequency * bandPass >> 15;
            FX::HARDCLIP::process(lowPass);
            
            int32_t highPass = notch - lowPass;
            bandPass += frequency * highPass >> 15;
            FX::HARDCLIP::process(bandPass);

            switch (_type) {
                case LowPass:
                    sample = lowPass;
                    break;
                case BandPass:
                    sample = bandPass;
                    break;
                case HighPass:
                    sample = highPass;
                    break;
                default:
                    sample = sample;
                    break;
            }
        }
    }
}
