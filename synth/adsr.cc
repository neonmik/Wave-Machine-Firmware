#include "adsr.h"

namespace ADSR {
    void Envelope::triggerAttack()  {
        currentFrame = 0;
        phase = Phase::ATTACK;
        endFrame = attack;
        increment = (int32_t(MAX_ATTACK) - int32_t(adsr)) / int32_t(endFrame);
    }
    void Envelope::triggerDecay() {
        currentFrame = 0;
        phase = Phase::DECAY;
        endFrame = decay;
        increment = (int32_t(sustain << 8) - int32_t(adsr)) / int32_t(endFrame);
    }
    void Envelope::triggerSustain() {
        currentFrame = 0;
        phase = Phase::SUSTAIN;
        endFrame = 0;
        increment = 0;
    }
    void Envelope::triggerRelease() {
        currentFrame = 0;
        phase = Phase::RELEASE;
        endFrame = release;
        increment = (int32_t(0) - int32_t(adsr)) / int32_t(endFrame);
    }

    void Envelope::stopped() {
        currentFrame = 0;
        phase = Phase::OFF;
        endFrame = 0;
        increment = 0;
        adsr = 0;
    }

    void Envelope::update() {
        if(phase == Phase::OFF) return;

        if ((currentFrame >= endFrame) && (phase != Phase::SUSTAIN)) {
            switch (phase) {
                case Phase::ATTACK:
                    triggerDecay();
                    break;
                case Phase::DECAY:
                    triggerSustain();
                    break;
                case Phase::RELEASE:
                    stopped();
                    break;
                default:
                    break;
                }
        }
        
        adsr += increment;
        ++currentFrame;

}
}