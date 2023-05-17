#pragma once

#include "pico/stdlib.h"


enum class Phase : uint8_t {
    ATTACK,
    DECAY,
    SUSTAIN,
    RELEASE,
    OFF
};

class ADSREnvelope {
    private:
        // uint32_t&   _sample_rate;

        uint32_t&   _attack;
        uint32_t&   _decay;
        uint32_t&   _sustain;
        uint32_t&   _release;
        
        uint32_t    _frame            = 0;      // number of frames into the current ADSR phase
        uint32_t    _end_frame        = 0;      // frame target at which the ADSR changes to the next phase
        uint32_t    _adsr             = 0;
        int32_t     _step             = 0;
        Phase       _phase            = Phase::OFF;

    public:

        ADSREnvelope(uint32_t& attack, uint32_t& decay, uint32_t& sustain, uint32_t& release) 
        : _attack(attack), _decay(decay), _sustain(sustain), _release(release) { }
        ~ADSREnvelope ( ) { }

        void trigger_attack();
        void trigger_decay();
        void trigger_sustain();
        void trigger_release();
        void stopped();
        
        void update(void);

        bool isStopped() { return _phase == Phase::OFF; }
        bool isReleasing() { return _phase == Phase::RELEASE; }

        uint32_t get_adsr() { return _adsr; }

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



  