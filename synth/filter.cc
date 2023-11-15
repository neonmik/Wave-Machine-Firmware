#include "filter.h"

namespace FILTER {

    ADSREnvelope ADSR{attack, decay, sustain, release};

    void init() {
        // lowPass = 0;
        // bandPass = 0;
        needsUpdating = true;
    }

    void setState (bool input) {
        state = input;
    }
    bool getState () {
        return state;
    }

    void setCutoff(uint16_t input) {
        cutoff = exponentialFrequency(input);
    }

    void setResonance(uint16_t input) {
        resonance = filter_damp(input);
        // needsUpdating = true;
    }


    void setPunch(uint16_t input) {
        punch = input >> 2;
    }


    void setType(uint16_t input) {
        uint8_t index = (input>>8);
        switch (index) {
        case 0:
            type = Type::Off;
            break;
        case 1:
            type = Type::LowPass;
            direction = Direction::Regular;
            break;
        case 2:
            type = Type::BandPass;
            direction = Direction::Regular;
            break;
        case 3:
            type = Type::HighPass;
            direction = Direction::Inverted;
            break;
        default:
            break;
        }
    }

    void setEnvelopeDepth (uint16_t input) {
        envelopeDepth = input;
    }

    void setDirection (uint16_t input) {
        uint8_t index = (input>>9);
        switch (index) {
        case 0:
            direction = Direction::Regular;
            break;
        case 1:
            direction = Direction::Inverted;
            break;
        default:
            break;
        }
    }

    void setTriggerMode (uint16_t input) {
        bool temp = (input >> 9);

        if (temp) {
            mode = Mode::PARA;
        } else {
            mode = Mode::MONO;
        }
    }


    void modulateCutoff(uint16_t input) {
        _mod = (input >> 2); 
        // _mod = input;
    }

    void setAttack (uint16_t input) {
        if (input == lastAttack) return;
        lastAttack = input;
        attack = calculateEndFrame(input << 2);
    }
    void setDecay (uint16_t input) {
        if (input == lastDecay) return;
        lastDecay = input;
        decay = calculateEndFrame(input << 2);
    }
    void setSustain (uint16_t input) {
        if (input == lastSustain) return;
        lastSustain = input;
        sustain = (input << 6);
    }
    void setRelease (uint16_t input) {
        if (input == lastRelease) return;
        lastRelease = input;
        release = calculateEndFrame(input << 2);
    }

    void voicesIncrease (void) {
        ++activeVoice;
        if (activeVoice > POLYPHONY) {
            activeVoice = POLYPHONY;
            DEBUG::print("Filter voices overflow!");
        }
    }
    void voicesDecrease (void) {
        --activeVoice;
        if (activeVoice < 0) {
            activeVoice = 0;
            DEBUG::print("Filter voices underflow!");
        }
    }
    bool voicesActive(void) {
        return activeVoice > 0;
    }

    void triggerAttack (void) {
        if (!voicesActive()) return;
        switch (mode) {
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
        if (!state) return;
        
        if ((type != Type::Off)) {
            ADSR.update();
            
            if (direction == Direction::Regular) {
                frequency = (cutoff * ADSR.get()) >> 16;
            } 
            if (direction == Direction::Inverted) {
                frequency = MAX_FREQ - (((MAX_FREQ - cutoff) * (ADSR.get())) >> 16);
            }

            frequency = frequency + _mod;
            // limiter for the modulation
            if (frequency > MAX_FREQ) frequency = MAX_FREQ;
            if (frequency < 0) frequency = 0;

            // Standard analogue synth envelope processing 
            // cutoff is the base line
            // EG amount is how far above the freq it reaches (upper limit should be NYQUIST)
            // Envelope amount should run from cutoff baseline, to max freq, then back down to wherever sustain level is set, then when released, drop to cutoff baseline
            // 
            // Frequency = Cutoff + (ADSR.get() * envelopDepth) // Need some bit shifts to correct for interger math here
            //



            int32_t damp = resonance;
            if (punch) {
                int32_t punch_signal = lowPass > 4096 ? lowPass : 2048;
                frequency += ((punch_signal >> 4) * punch) >> 9;
                damp += ((punch_signal - 2048) >> 3);
            }

            int32_t notch = sample - (bandPass * damp >> 15);
            lowPass += frequency * bandPass >> 15;
            FX::HARDCLIP::process(lowPass);
            
            int32_t highPass = notch - lowPass;
            bandPass += frequency * highPass >> 15;
            FX::HARDCLIP::process(bandPass);

            switch (type) {
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
