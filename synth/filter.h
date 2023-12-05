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

        uint16_t    MAX_FREQ = NYQUIST;

        int32_t     cutoff;
        int32_t     resonance;
        
        uint16_t    envelopeDepth;
        // uint16_t    keyboardDepth;


        uint16_t    _mod;

        int32_t     punch;
        int32_t     frequency;
        int32_t     damp;

        int32_t     lowPass;
        int32_t     bandPass;

        Type        type;
        Direction   direction;

        volatile int8_t activeVoice;
        bool filterActive = false;


        ADSRControls    envelopeControls(SAMPLE_RATE);
    }

    extern ADSREnvelope cutoffEnvelope;

    void init();

    void process(int32_t &sample);

    void setState(bool input);
    bool getState (void);

    void setCutoff(uint16_t input);
    void setResonance(uint16_t input);
    void setPunch(uint16_t input);
    void setType(uint16_t input);

    void setEnvelopeAmount (uint16_t input) ;

    void setTriggerMode (uint16_t input);

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
