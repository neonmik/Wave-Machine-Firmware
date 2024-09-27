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
