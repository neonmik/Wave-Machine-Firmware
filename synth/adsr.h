#pragma once

#include "config.h"

#define MAX_ATTACK  0xFFFFFF

enum class Phase : uint8_t {
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    OFF
};

enum class Mode
{
    MONO,
    PARA,
};


namespace ADSR {
    struct Parameters {
        uint32_t attack;
        uint32_t decay;
        uint32_t sustain;
        uint32_t release;
    };
    class Controls {
        private:
            uint32_t        sampleRate;

            Parameters      envelopeParameters;

            uint32_t calculateEndFrame(uint32_t milliseconds){
                return ((milliseconds + 1) * sampleRate) / 1000;
            }

        public: 
            Controls (uint32_t sampleRate) : sampleRate(sampleRate) { }
            ~Controls ( ) { }

            void setAttack (const uint16_t& input) {
                envelopeParameters.attack = calculateEndFrame(input << 2);
            }
            void setDecay (const uint16_t& input) {
                envelopeParameters.decay = calculateEndFrame(input << 2);
            }
            void setSustain (const uint16_t& input) {
                envelopeParameters.sustain = (input << 6); // bit shift to make it 16 bit - change if we want to change the resolution
            }
            void setRelease (const uint16_t& input) {
                envelopeParameters.release = calculateEndFrame(input << 2);
            }

            const uint32_t& getAttack (void) {
                return envelopeParameters.attack;
            }
            const uint32_t& getDecay (void) {
                return envelopeParameters.decay;
            }
            const uint32_t& getSustain (void) {
                return envelopeParameters.sustain;
            }
            const uint32_t& getRelease (void) {
                return envelopeParameters.release;
            }
    };
    class Envelope {
        private:

            const uint32_t& attack;
            const uint32_t& decay;
            const uint32_t& sustain;
            const uint32_t& release;

            
            uint32_t    currentFrame        = 0;
            uint32_t    endFrame            = 0;
            uint32_t    adsr                = 0;
            int32_t     increment           = 0;
            Phase       phase               = Phase::OFF;

        public:

            Envelope(const uint32_t& attack, const uint32_t& decay, const uint32_t& sustain, const uint32_t& release)
                    : attack(attack), decay(decay), sustain(sustain), release(release) { }

            ~Envelope ( ) { }

            void triggerAttack(void);
            void triggerDecay(void);
            void triggerSustain(void);
            void triggerRelease(void);
            void stopped(void);
            
            void update(void);

            bool isStopped(void) { return phase == Phase::OFF; }
            bool isReleasing(void) { return phase == Phase::RELEASE; }

            uint32_t get(void) { return (adsr >> 8); }

            int32_t apply(uint32_t input) {;
                return ((int32_t(input) * int32_t(adsr >> 8))) >> 16;
            }

    };
};




// The duration a note is played is determined by the amount of attack,
// decay, and release, combined with the length of the note as defined by
// the user.
//
// - Attack:  number of milliseconds it takes for a note to hit full volume
// - Decay:   number of milliseconds it takes for a note to settle to sustain volume
// - Sustain: percentage of full volume that the note sustains at (duration implied by held note)
// - Release: number of milliseconds it takes for a note to reduce to zero volume after it has ended
//
// Attack (750ms) - Decay (500ms) -------- Sustain ----- Release (250ms)
//
//                +         +                                  +    +
//                |         |                                  |    |
//                |         |                                  |    |
//                |         |                                  |    |
//                v         v                                  v    v
// 0ms               1000ms              2000ms              3000ms              4000ms
//
// |              XXXX |                   |                   |                   |
// |             X    X|XX                 |                   |                   |
// |            X      |  XXX              |                   |                   |
// |           X       |     XXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX|                   |
// |          X        |                   |                   |X                  |
// |         X         |                   |                   |X                  |
// |        X          |                   |                   | X                 |
// |       X           |                   |                   | X                 |
// |      X            |                   |                   |  X                |
// |     X             |                   |                   |  X                |
// |    X              |                   |                   |   X               |
// |   X               |                   |                   |   X               |
// |  X +    +    +    |    +    +    +    |    +    +    +    |    +    +    +    |    +
// | X  |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
// |X   |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |
// +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+--->
