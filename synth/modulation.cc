#include "modulation.h"


namespace MOD {

    void  Modulation::init (void) {

    }
    // control and update functions
    void Modulation::setState (bool state) {
        if (state != state) {
            state = state;
            reset(); // aim to use this to ramp down values when switching states
        }
    }
    bool Modulation::getState (void) {
        return state;
    }
    
    void Modulation::checkMatrix (void) {
        if (matrix != lastMatrix) {
            index = 0;
            phaseAccumulator = 0;
            // update the previous destination output to the offset position
            resetDestination(lastMatrix);
            resetDestination(matrix);

            lastMatrix = matrix;
        }
    }
    
    void Modulation::update () {
        if (state) {
            checkMatrix();
            outputEnvelope.update();

            phaseAccumulator += increment; // Adds the increment to the accumulator
            index = (phaseAccumulator >> Speed::SLOW);    // Calculates the 8 bit index value for the wavetable. the bit shift creates diffeing results... see LFO_SPEED table
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
            
            // two different algorithms for applying depth to the outputs, ensures always the number is centred round the appropriate 0 mark for the destination. 
            switch (destination[matrix].type) {
                case OutputType::UNSIGNED: {
                    destination[matrix].output = (uint16_output(sample) * outputEnvelope.apply(depth)) >> 10;
                    break;
                }
                case OutputType::SIGNED: {
                    destination[matrix].output = uint16_output((sample * outputEnvelope.apply(depth)) >> 10);
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
    // Modulation LFO(SAMPLE_RATE/8); // reduced speed poly LFO
    Modulation LFO(shape, rate, depth, outputMatrix); // full speed mono LFO


    void init () {
        LFO.init();

        outputMatrix = 0;

        setAttack(800);
        setDecay(800);
        setSustain(200);
        setRelease(800);
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

        // TODO: Implement a way of using EG, any selected wave, or Noise as the mod source here.
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
        // if (input == lastAttack) return;
        // lastAttack = input;
        // attack = calculateEndFrame(input << 2);
    }
    void setDecay (uint16_t input) {
        envelopeControls.setDecay(input);
        // if (input == lastDecay) return;
        // lastDecay = input;
        // decay = calculateEndFrame(input << 2);
    }
    void setSustain (uint16_t input) {
        envelopeControls.setSustain(input);
        // if (input == lastSustain) return;
        // lastSustain = input;
        // sustain = (input << 6);
    }
    void setRelease (uint16_t input) {
        envelopeControls.setRelease(input);
        // if (input == lastRelease) return;
        // lastRelease = input;
        // release = calculateEndFrame(input << 2);
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