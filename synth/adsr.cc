#include "adsr.h"

void ADSR::trigger_attack()  {
    frame = 0;
    phase = Phase::ATTACK;
    end_frame = (attack_ms * _sample_rate) / 1000;
    step = (int32_t(0xffffff) - int32_t(adsr)) / int32_t(end_frame);
}
void ADSR::trigger_decay() {
    frame = 0;
    phase = Phase::DECAY;
    end_frame = (decay_ms * _sample_rate) / 1000;
    step = (int32_t(sustain << 8) - int32_t(adsr)) / int32_t(end_frame);
}
void ADSR::trigger_sustain() {
    frame = 0;
    phase = Phase::SUSTAIN;
    end_frame = 0;
    step = 0;
}
void ADSR::trigger_release() {
    frame = 0;
    phase = Phase::RELEASE;
    end_frame = (release_ms * _sample_rate) / 1000;
    step = (int32_t(0) - int32_t(adsr)) / int32_t(end_frame);
}

void ADSR::off() {
    frame = 0;
    phase = Phase::OFF;
    end_frame = 0;
    step = 0;
}

void ADSR::init(uint32_t sample_rate) {
    _sample_rate = sample_rate;
    off();
}

void ADSR::update() {
    if ((frame >= end_frame) && (phase != Phase::SUSTAIN)) {
        switch (phase) {
          case Phase::ATTACK:
            trigger_decay();
            break;
          case Phase::DECAY:
            trigger_sustain();
            break;
          case Phase::RELEASE:
            off();
            break;
          default:
            break;
        }
      }
}