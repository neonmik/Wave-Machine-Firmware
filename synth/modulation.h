#pragma once

#include "../config.h"
#include "../functions.h"

#include "../random.h"


#include "synth.h"
#include "arp.h"
#include "filter.h"
#include "adsr.h"

#include "wavetable.h"

namespace MOD {
    enum Mode
    {
        MONO,
        PARA,
    };
    namespace {
        uint16_t    shape;
        uint32_t    rate;
        uint16_t    depth;
        uint8_t     outputMatrix;

        uint32_t incrementCalc (uint16_t input) {
            return (65535 * uint32_t(exponentialFrequency(input))) / SAMPLE_RATE;
        }

        ADSRControls    envelopeControls(SAMPLE_RATE);

        Mode            mode = Mode::MONO;
        volatile int8_t activeVoice;
        bool            filterActive = false;

    }

    enum Speed {                        // Hz = 1 (cycles) / seconds per full cycle
                    PAINFUL = 16,           // 0.00286Hz -   38.46Hz (5:49:497  - 0:00:0260)
                    SLOWER  = 15,           // 0.00572Hz -   76.92Hz (5:49:497  - 0:00:0130)
                    SLOW    = 14,           // 0.01144Hz -  153.84Hz (1:27:363  - 0:00:0065)
                    NORMAL  = 13,           // 0.02289Hz -  307.69Hz (0:43:628  - 0:00:00325)
                    FAST    = 12,           // 0.04578Hz -  615.38Hz (0:21:814  - 0:00:001625)
                    FASTER  = 11,           // 0.09156Hz - 1230.77Hz (0:10:907  - 0:00:0008125)
                    SILLY   = 10            // 0.18312Hz - 2461.54Hz (0:05.453  - 0:00:00040625)
    };

    enum Dither : uint8_t {
        FULL,
        HALF,
        LOW,
        OFF
    };

    enum OutputType : uint8_t {
        SIGNED,
        UNSIGNED
    };

    struct OutputDestinations {
        void            (*variable)(uint16_t);
        OutputType      type;
        Dither          dither;
        uint16_t        output = 0;
    };

    
    class Modulation {
        private:
            // oscillator variables
            bool            state = true;

            uint16_t&       wave;
            uint32_t&       increment;
            uint16_t&       depth;
            uint8_t&        matrix;
            uint8_t         lastMatrix;


            uint32_t        phaseAccumulator;
            uint8_t         index;

            int16_t         sample;

            

            
            OutputDestinations destination[4]{
                // pointer of what to update, type of output, offset for output table
                {&SYNTH::modulateVibrato,  OutputType::SIGNED,     Dither::FULL},
                {&SYNTH::modulateTremelo,  OutputType::UNSIGNED,   Dither::LOW},
                {&SYNTH::modulateVector,   OutputType::UNSIGNED,   Dither::LOW},
                {&FILTER::modulateCutoff,  OutputType::UNSIGNED,   Dither::LOW} 
            };

            void checkMatrix (void) {
                if (matrix != lastMatrix) {
                    index = 0;
                    phaseAccumulator = 0;
                    // update the previous destination output to the offset position
                    resetDestination(lastMatrix);
                    resetDestination(matrix);

                    lastMatrix = matrix;
                }
            }

            inline uint16_t uint16_output (int16_t input) {
                return input - INT16_MIN; // Using modular arithmatic!
            }

            void resetDestination (uint8_t index) {
                if (destination[index].variable != NULL) {
                    switch (destination[index].type) {
                        case OutputType::UNSIGNED:
                            destination[index].variable(0);
                            break;
                        case OutputType::SIGNED:
                            destination[index].variable(uint16_output(0));
                    }
                }
            }

            void reset (void) {
                index = 0;
                phaseAccumulator = 0;
                sample = 0;

                for (int i = 0; i < 3; i++) {
                    resetDestination(i);
                }
            }
            
        public:
            Modulation (uint16_t& shape, uint32_t& rate, uint16_t& depth, uint8_t& matrix) : wave(shape), increment(rate), depth(depth), matrix(matrix) { }
            ~Modulation( ) { }
            
            ADSREnvelope outputEnvelope{envelopeControls.getAttack(), envelopeControls.getDecay(), envelopeControls.getSustain(), envelopeControls.getRelease()};

            void init (void);
            void setState (bool state);
            bool getState (void);
            void update (void);
            void clear (void);
    };

    extern Modulation LFO;

    void init (void);

    void setState (bool input);

    void setMatrix (uint16_t input);
    void setDepth (uint16_t input);
    void setRate (uint16_t input);
    void setShape (uint16_t input);

    void setAttack(uint16_t input);
    void setDecay(uint16_t input);
    void setSustain(uint16_t input);
    void setRelease(uint16_t input);

    void triggerAttack(void);
    void triggerRelease(void);

    void voicesIncrease(void);
    void voicesDecrease(void);

    void update (void);
    void clear (void);
}



