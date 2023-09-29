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

class ADSREnvelope {
    private:
        // uint32_t   sampleRate;

        uint32_t&    attack;
        uint32_t&    decay;
        uint32_t&    sustain;
        uint32_t&    release;
        // uint32_t    lastAttack;
        // uint32_t    lastDecay;
        // uint32_t    lastSustain;
        // uint32_t    lastRelease;
        
        uint32_t    currentFrame        = 0;
        uint32_t    endFrame            = 0;
        uint32_t    adsr                = 0;
        int32_t     increment           = 0;
        Phase       phase               = Phase::OFF;

        // uint32_t calculateEndFrame(uint32_t milliseconds){
        //     return ((milliseconds + 1) * SAMPLE_RATE) / 1000;
        // }

    public:

        ADSREnvelope(uint32_t& attack, uint32_t& decay, uint32_t& sustain, uint32_t& release) 
        : attack(attack), decay(decay), sustain(sustain), release(release) { }
        // ADSREnvelope(uint32_t samplerate) 
        ~ADSREnvelope ( ) { }

        // void setAttack (uint16_t input);
        // void setDecay (uint16_t input);
        // void setSustain (uint16_t input);
        // void setRelease (uint16_t input);

        void triggerAttack(void);
        void triggerDecay(void);
        void triggerSustain(void);
        void triggerRelease(void);
        void stopped(void);
        
        void update(void);

        bool isStopped(void) { return phase == Phase::OFF; }
        bool isReleasing(void) { return phase == Phase::RELEASE; }

        uint32_t get(void) { return (adsr >> 8); }

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



  