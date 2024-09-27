#include "filter.h"


#include "Filter.h"
#include "Debug.h" // Assuming you have this header for debug prints

namespace FILTER {

    StateVariable filter[8];

    void init() {
        for (int i = 0; i < POLYPHONY; i++) {
            filter[i].init(i);
        }
    }


    void setAttack(uint16_t input)  {   envelopeControls.setAttack(input);  }
    void setDecay(uint16_t input)   {   envelopeControls.setDecay(input);   }
    void setSustain(uint16_t input) {   envelopeControls.setSustain(input); }
    void setRelease(uint16_t input) {   envelopeControls.setRelease(input); }



    void triggerAttack(uint8_t voice)   {   filter[voice].triggerAttack();  }
    void triggerRelease(uint8_t voice)  {   filter[voice].triggerRelease(); }

    void update(uint8_t voice) {    filter[voice].update(); }

    void process(uint8_t voice, int32_t &sample) {
        if (!state) return;

        filter[voice].process(sample);
    }


    void setState(bool input) { 
        for (int i = 0; i < POLYPHONY; i++) {
            filter[i].reset();
        }
        state = input;
    }

    bool getState(void) { 
        return state; 
    }

    void setCutoff(uint16_t input)      {   cutoff = exponentialFrequency(input);   }
    void setResonance(uint16_t input)   {   resonance = filterDamp(input);          }
    void setPunch(uint16_t input)       {   punch = input >> 2;                     }

    void setType(uint16_t input) {
        uint8_t index = (input >> 8);
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

    void setEnvelopeDepth(uint16_t input)   {   envelopeDepth = input;  }
    void setDirection(uint16_t input)       {
        uint8_t index = (input >> 9);
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

    void modulateCutoff(uint16_t input) {   modulation = (input >> 2);  }

}

// uint16_t Filter::exponentialFrequency(uint16_t input) {
//     // Implement exponential frequency calculation
//     return input;
// }
// uint16_t Filter::filterDamp(uint16_t input) {
//     // Implement filter damping calculation
//     return input;
// }
// namespace FILTER {
//     void init() {
//         reset();
//     }

//     void setState(bool input) { 
//         reset();
//         state = input;
//     }

//     bool getState (void) { return state; }

//     void setCutoff(uint16_t input) {
//         cutoff = exponentialFrequency(input);
//     }

//     void setResonance(uint16_t input) {
//         resonance = filterDamp(input);
//     }


//     void setPunch(uint16_t input) {
//         punch = input >> 2;
//     }


//     void setType(uint16_t input) {
//         uint8_t index = (input>>8);
//         switch (index) {
//         case 0:
//             type = Type::Off;
//             break;
//         case 1:
//             type = Type::LowPass;
//             direction = Direction::Regular;
//             break;
//         case 2:
//             type = Type::BandPass;
//             direction = Direction::Regular;
//             break;
//         case 3:
//             type = Type::HighPass;
//             direction = Direction::Inverted;
//             break;
//         default:
//             break;
//         }
//     }

//     void setEnvelopeAmount (uint16_t input) {
//         envelopeDepth = input;
//     }

//     void setDirection (uint16_t input) {
//         uint8_t index = (input>>9);
//         switch (index) {
//         case 0:
//             direction = Direction::Regular;
//             break;
//         case 1:
//             direction = Direction::Inverted;
//             break;
//         default:
//             break;
//         }
//     }

//     void setTriggerMode (uint16_t input) {
//         if (input >> 9) {
//             mode = Mode::PARA;
//         } else {
//             mode = Mode::MONO;
//         }
//     }


//     void modulateCutoff(uint16_t input) {
//         modulation = (input >> 2); 
//     }

//     void setAttack (uint16_t input) {
//         envelopeControls.setAttack(input);
//     }
//     void setDecay (uint16_t input) {
//         envelopeControls.setDecay(input);
//     }
//     void setSustain (uint16_t input) {
//         envelopeControls.setSustain(input);
//     }
//     void setRelease (uint16_t input) {
//         envelopeControls.setRelease(input);
//     }

//     void voicesIncrease (void) {
//         ++activeVoice;
//         if (activeVoice > POLYPHONY) {
//             activeVoice = POLYPHONY;
//             DEBUG::print("Filter voices overflow!");
//         }
//     }
//     void voicesDecrease (void) {
//         --activeVoice;
//         if (activeVoice < 0) {
//             activeVoice = 0;
//             DEBUG::print("Filter voices underflow!");
//         }
//     }
//     bool voicesActive(void) {
//         return activeVoice > 0;
//     }

//     void triggerAttack (void) {
//         if (!voicesActive()) return;
//         switch (mode) {
//             case Mode::MONO:
//                 if (!filterActive) { 
//                     cutoffEnvelope.triggerAttack();
//                     filterActive = true;
//                 }
//                 break;
//             case Mode::PARA:
//                 cutoffEnvelope.triggerAttack();
//                 filterActive = true;
//                 break;
//         } 
        
//     }
//     void triggerRelease (void) {
//         if (filterActive && !voicesActive()) {
//             cutoffEnvelope.triggerRelease();
//             filterActive = false;
//         }
//     }

    
//     void process(int32_t &sample) {
//         if (!state) return;
        
//         if ((type != Type::Off)) {
//             cutoffEnvelope.update();
            
//             if (direction == Direction::Regular) {
//                 frequency = (cutoff * cutoffEnvelope.get()) >> 16;
//             } 
//             if (direction == Direction::Inverted) {
//                 frequency = NYQUIST - (((NYQUIST - cutoff) * (cutoffEnvelope.get())) >> 16);
//             }

//             frequency = frequency + modulation;
//             // limiter for the modulation
//             if (frequency > NYQUIST) frequency = NYQUIST;
//             if (frequency < 0) frequency = 0;

//             // Standard analogue synth envelope processing 
//             // cutoff is the base line
//             // EG amount is how far above the freq it reaches (upper limit should be NYQUIST)
//             // Envelope amount should run from cutoff baseline, to max freq, then back down to wherever sustain level is set, then when released, drop to cutoff baseline
//             // 
//             // Frequency = Cutoff + (cutoffEnvelope.get() * envelopDepth) // Need some bit shifts to correct for integer math here
//             //



//             int32_t damp = resonance;
//             if (punch) {
//                 int32_t punch_signal = lowPass > 4096 ? lowPass : 2048;
//                 frequency += ((punch_signal >> 4) * punch) >> 9;
//                 damp += ((punch_signal - 2048) >> 3);
//             }

//             int32_t notch = sample - (bandPass * damp >> 15);
//             lowPass += frequency * bandPass >> 15;
//             FX::HARDCLIP::process(lowPass);
            
//             int32_t highPass = notch - lowPass;
//             bandPass += frequency * highPass >> 15;
//             FX::HARDCLIP::process(bandPass);

//             switch (type) {
//                 case LowPass:
//                     sample = lowPass;
//                     break;
//                 case BandPass:
//                     sample = bandPass;
//                     break;
//                 case HighPass:
//                     sample = highPass;
//                     break;
//                 default:
//                     sample = sample;
//                     break;
//             }
//         }
//     }
// }
