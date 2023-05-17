#include "adsr.h"

void ADSREnvelope::trigger_attack()  {
    _frame = 0;
    _phase = Phase::ATTACK;
    _end_frame = _attack;
    _step = (int32_t(0xffffff) - int32_t(_adsr)) / int32_t(_end_frame);
}
void ADSREnvelope::trigger_decay() {
    _frame = 0;
    _phase = Phase::DECAY;
    _end_frame = _decay;
    _step = (int32_t(_sustain << 8) - int32_t(_adsr)) / int32_t(_end_frame);
}
void ADSREnvelope::trigger_sustain() {
    _frame = 0;
    _phase = Phase::SUSTAIN;
    _end_frame = 0;
    _step = 0;
}
void ADSREnvelope::trigger_release() {
    _frame = 0;
    _phase = Phase::RELEASE;
    _end_frame = _release;
    _step = (int32_t(0) - int32_t(_adsr)) / int32_t(_end_frame);
}

void ADSREnvelope::stopped() {
    _frame = 0;
    _phase = Phase::OFF;
    _end_frame = 0;
    _step = 0;
}

void ADSREnvelope::update() {
    if(_phase == Phase::OFF) {
        return;
    } 
    if ((_frame >= _end_frame) && (_phase != Phase::SUSTAIN)) {
        switch (_phase) {
            case Phase::ATTACK:
                trigger_decay();
                break;
            case Phase::DECAY:
                trigger_sustain();
                break;
            case Phase::RELEASE:
                stopped();
                break;
            default:
                break;
            }
    }
    
    _adsr += _step;
    _frame++;
}