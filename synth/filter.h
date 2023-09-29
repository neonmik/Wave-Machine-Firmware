#pragma once


#include "../config.h"
#include "../functions.h"

#include "../queue.h"

#include "resources.h"

#include "adsr.h"
#include "fx.h"

namespace FILTER
{

    enum Type
    {
        Off,
        LowPass,
        BandPass,
        HighPass
    };

    enum Direction
    {
        Regular,
        Inverted
    };

    enum Mode
    {
        MONO,
        PARA,
    };

    namespace
    {
        bool        state;
        Mode        mode = Mode::PARA;

        bool        needsUpdating;

        int16_t     cutoff;
        int16_t     resonance;

        uint16_t    _mod;

        int32_t     punch;
        int32_t     frequency;
        int32_t     damp;

        int32_t     lowPass;
        int32_t     bandPass;

        uint32_t    attack;
        uint32_t    decay;
        uint32_t    sustain;
        uint32_t    release;
        uint16_t    lastAttack = 1024;
        uint16_t    lastDecay = 1024;
        uint16_t    lastSustain = 1024;
        uint16_t    lastRelease = 1024;

        Type        type;
        Direction   direction;

        uint32_t calculateEndFrame(uint32_t milliseconds){
            // return (milliseconds * (SAMPLE_RATE/8)) / 1000;
            return ((milliseconds + 1) * SAMPLE_RATE) / 1000;
        }

        volatile int8_t activeVoice;
        bool filterActive = false;

        // TODO: remove this, don't need this function anymore
        // void voicesIncrease(void) {
        //     ++activeVoice;
        //     if (activeVoice > POLYPHONY)
        //     {
        //         activeVoice = POLYPHONY;
        //     }
        // }
        // void voicesDecrease(void) {
        //     --activeVoice;
        //     if (activeVoice < 0)
        //     {
        //         activeVoice = 0;
        //     }
        // }
        // void voicesClear(void) {
        //     activeVoice = 0;
        // }
        

    }

    extern ADSREnvelope ADSR;

    void init();

    void process(int32_t &sample);

    void setState(bool input);
    bool getState (void);

    void setCutoff(uint16_t input);
    void setResonance(uint16_t input);
    void setPunch(uint16_t input);
    void setType(uint16_t input);

    void setAttack(uint16_t input);
    void setDecay(uint16_t input);
    void setSustain(uint16_t input);
    void setRelease(uint16_t input);

    void voicesIncrease(void);
    void voicesDecrease(void);

    void triggerAttack(void);
    void triggerRelease(void);

    void modulateCutoff(uint16_t input);
    void modulateResonance(uint16_t input);

}
