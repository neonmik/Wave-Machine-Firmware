#include "modulation.h"


namespace MOD {

    void  Modulation::init (void) {

    }
    // control and update functions
    void Modulation::setState (bool input) {
        if (state != input) {
            state = input;
            reset(); // aim to use this to ramp down values when switching states
        }
    }
    bool Modulation::getState (void) {
        return state;
    }
    void Modulation::update () {
        if (state) {
            checkMatrix();
            // outputEnvelope.update();
            
            // rough rate envelope
            // increment = outputEnvelope.apply(increment);

            phaseAccumulator += increment; // Adds the increment to the accumulator
            index = (phaseAccumulator >> Speed::PAINFUL);    // Calculates the 8 bit index value for the wavetable. the bit shift creates differing results... see LFO_SPEED table
            sample = get_mod_wavetable(index + wave); // Sets the wavetable value to the sample by using a combination of the index (0-255) and wave (chunks of 256) values
            
            // Applies a certain dither to the output - really just for smoothing out 8 bit numbers over 0.01Hz, but interesting for effects.
            switch (destination[matrix].dither) {
                case Dither::FULL:
                    if (sample > 0) sample -= (RANDOM::get()>>4);
                    else sample += (RANDOM::get()>>4);
                    break;
                case Dither::HALF:
                    if (sample > 0) sample -= (RANDOM::get()>>6);
                    else sample += (RANDOM::get()>>6);
                    break;
                case Dither::LOW:
                    if (sample > 0) sample -= (RANDOM::get()>>7);
                    else sample += (RANDOM::get()>>7);
                    break;
                case Dither::OFF:
                    break;
            }
            
            // Depth Envelope:
            // sample = outputEnvelope.apply(sample);

            // two different algorithms for applying depth to the outputs, ensures always the number is centred round the appropriate 0 mark for the destination. 
            switch (destination[matrix].type) {
                case OutputType::UNSIGNED: {
                    destination[matrix].output = (uint16_output(sample) * depth) >> 10;
                    break;
                }
                case OutputType::SIGNED: {
                    destination[matrix].output = uint16_output((sample * depth) >> 10);
                    break;
                }
            }

            

            if (destination[matrix].variable != NULL)
                    if (depth) destination[matrix].variable(destination[matrix].output);
        }
    }
    void Modulation::clear (void) {
        reset();
    }
    
    Modulation LFO(shape, rate, depth, outputMatrix); // full speed mono LFO


    void init () {
        LFO.init();

        outputMatrix = 0;

        setAttack(0);
        setDecay(0);
        setSustain(1023);
        setRelease(0);
    }
    void update () {
        LFO.update();
    }
    void clear () {
        LFO.clear();
    }

    void setState (bool input) {
        LFO.setState(input);
    }
    
    void setShape (uint16_t input) {
        shape = (map(input, KNOB_MIN, KNOB_MAX, 0, (MAX_MOD_WAVES - 1)) << 8);

        // TODO: #10 Implement a way of using any selected wave/ADSR/Noise/Sample and Hold as the mod source here.
    }
    void setDepth (uint16_t input) {
        if (depth != input) {
            depth = input;
        }
    }
    void setRate (uint16_t input) {
        rate = incrementCalc(input);
        if (rate < 1) rate = 1;
    }
    void setMatrix (uint16_t input) {
        outputMatrix = input >> 8;
    }

    void setAttack (uint16_t input) {
        envelopeControls.setAttack(input);
    }
    void setDecay (uint16_t input) {
        envelopeControls.setDecay(input);
    }
    void setSustain (uint16_t input) {
        envelopeControls.setSustain(input);
    }
    void setRelease (uint16_t input) {
        envelopeControls.setRelease(input);
    }

     void voicesIncrease (void) {
        ++activeVoice;
        if (activeVoice > POLYPHONY) {
            activeVoice = POLYPHONY;
            DEBUG::print("Mod voices overflow!");
        }
    }
    void voicesDecrease (void) {
        --activeVoice;
        if (activeVoice < 0) {
            activeVoice = 0;
            DEBUG::print("Mod voices underflow!");
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
                    LFO.outputEnvelope.triggerAttack();
                    filterActive = true;
                }
                break;
            case Mode::PARA:
                LFO.outputEnvelope.triggerAttack();
                filterActive = true;
                break;
        } 
        
    }
    void triggerRelease (void) {
        if (filterActive && !voicesActive()) {
            LFO.outputEnvelope.triggerRelease();
            filterActive = false;
        }
    }


}