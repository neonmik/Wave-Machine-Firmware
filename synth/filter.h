#pragma once


#include "../config.h"
#include "../functions.h"

#include "../queue.h"

#include "resources.h"

#include "adsr.h"
#include "fx.h"

namespace FILTER{
    
    enum class Type {
        Off,
        LowPass,
        BandPass,
        HighPass
    };

    enum class Direction {
        Regular,
        Inverted
    };

    static ADSR::Controls    envelopeControls(SAMPLE_RATE);

    namespace {

        bool state;

        uint32_t cutoff;
        uint32_t resonance;
        uint32_t punch;
        Type type;

        int16_t envelopeDepth;
        Direction direction;
        int32_t modulation;
    }

    class StateVariable {
    public:
        

        StateVariable() { };

        void init(uint8_t voice) {
            voiceIndex = voice;
            reset();
            cutoffEnvelope.stopped();
        }
        void triggerAttack(void) {
            active = true;
            cutoffEnvelope.triggerAttack();
        }
        void triggerRelease(void) {
            cutoffEnvelope.triggerRelease();
        }

        bool isStopped(void) {
            return !active;
        }

        void update(void) {
            cutoffEnvelope.update();
        }

        void process(int32_t &sample) {
            if ((type != Type::Off)) {

                if (cutoffEnvelope.isStopped() && active) {
                    triggerStopped();
                }
                
                if (direction == Direction::Regular) {
                    frequency = (cutoff * cutoffEnvelope.get()) >> 16;
                } 
                if (direction == Direction::Inverted) {
                    frequency = NYQUIST - (((NYQUIST - cutoff) * (cutoffEnvelope.get())) >> 16);
                }

                // TODO: #8 Implement correct use of envelope depth to control ADSR in relation to cutoff
                // int32_t envDep = (((int32_t)cutoffEnvelope.get() * (int32_t)envelopeDepth) >> 9);
                // frequency = (int32_t)cutoff + envDep;

                frequency = frequency + (cutoffEnvelope.get() * envelopeDepth) >> 9;

                frequency = frequency + modulation;
                // limiter for the modulation
                if (frequency > NYQUIST) frequency = NYQUIST;
                if (frequency < 0) frequency = 0;

                int32_t damp = resonance;

                //Punch was a feature ported from some external code that I couldnt hear, so I've removed it for more useful features.
                // if (punch) {
                //     int32_t punch_signal = lowPass > 4096 ? lowPass : 2048;
                //     frequency += ((punch_signal >> 4) * punch) >> 9;
                //     damp += ((punch_signal - 2048) >> 3);
                // }

                int32_t notch = sample - (bandPass * damp >> 15);
                lowPass += frequency * bandPass >> 15;
                FX::HARDCLIP::process(lowPass);
                
                int32_t highPass = notch - lowPass;
                bandPass += frequency * highPass >> 15;
                FX::HARDCLIP::process(bandPass);

                switch (type) {
                    case Type::LowPass:
                        sample = lowPass;
                        break;
                    case Type::BandPass:
                        sample = bandPass;
                        break;
                    case Type::HighPass:
                        sample = highPass;
                        break;
                    default:
                        sample = sample;
                        break;
                }
            }
        }

        void reset(void) {
            lowPass = 0;
            bandPass = 0;
        }

    private:
        ADSR::Envelope    cutoffEnvelope{envelopeControls.getAttack(), envelopeControls.getDecay(), envelopeControls.getSustain(), envelopeControls.getRelease()};
        
        void triggerStopped(void) {
            active = false;
        }

        uint8_t voiceIndex;

        int32_t frequency;
        int32_t lowPass;
        int32_t bandPass;

        bool active;

    };

    void init();

    void setAttack(uint16_t input);
    void setDecay(uint16_t input);
    void setSustain(uint16_t input);
    void setRelease(uint16_t input);

    void triggerAttack(uint8_t voice);
    void triggerRelease(uint8_t voice);

    void update(uint8_t voice);

    void process(uint8_t voice, int32_t&sample);

    void setState(bool input);
    bool getState(void);

    void setCutoff(uint16_t input);
    void setResonance(uint16_t input);
    void setPunch(uint16_t input);
    void setType(uint16_t input);

    void setEnvelopeDepth(uint16_t input);
    void setDirection(uint16_t input);

    void modulateCutoff(uint16_t input);
}
