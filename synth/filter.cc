#include "filter.h"

namespace FILTER {
    void init() {
        lp_ = 0;
        bp_ = 0;
        frequency_ = 33 << 7;
        resonance_ = 16384;
        dirty_ = true;
        punch_ = 0;
        mode_ = LowPass;
    }

    void set_frequency(uint16_t frequency) {
        int16_t temp = map_exp(frequency, KNOB_MIN, KNOB_MAX, 1, 10000); // seems 0-16000 is functional range...
        dirty_ = dirty_ || (frequency_ != temp);
        frequency_ = temp;
    }

    void set_resonance(uint16_t resonance) {
        int temp = map(resonance, KNOB_MIN, KNOB_MAX, 0, INT16_MAX);
        resonance_ = temp;
        dirty_ = true;
    }

    void set_punch(uint16_t punch) {
        // punch_ = (static_cast<uint32_t>(punch) * punch) >> 24;
        punch_ = (punch >> 4);
    }

    void set_mode(FilterType mode) {
        mode_ = mode;
    }

    int32_t process(int32_t input) {
        
        if (dirty_) {
            f_ = Interpolate824(lut_svf_cutoff, frequency_ << 17);
            damp_ = Interpolate824(lut_svf_damp, resonance_ << 17);
            dirty_ = false;
        }
        int32_t f = f_;
        int32_t damp = damp_;
        if (punch_) {
            int32_t punch_signal = lp_ > 4096 ? lp_ : 2048;
            f += ((punch_signal >> 4) * punch_) >> 9;
            damp += ((punch_signal - 2048) >> 3);
        }

        int32_t notch = input - (bp_ * damp >> 15);
        lp_ += f * bp_ >> 15;
        CLIP(lp_)
        int32_t hp = notch - lp_;
        bp_ += f * hp >> 15;
        CLIP(bp_)
        return mode_ == BandPass ? bp_ : (mode_ == HighPass ? hp : lp_);
    }




//     int32_t process(int32_t input) {
//     if (dirty_) {
//         f_ = Interpolate824(lut_svf_cutoff, frequency_ << 17);
//         damp_ = Interpolate824(lut_svf_damp, resonance_ << 17);
//         dirty_ = false;
//     }
//     int32_t f = f_;
//     int32_t damp = damp_;
//     for (int i = 0; i < 4; i++) {
//         // Gives a bump in the low end, disable by setting punch_ = 0 for now.
//         if (punch_) {
//             int32_t punch_signal = lp_ > 4096 ? lp_ : 2048;
//             f += ((punch_signal >> 4) * punch_) >> 9;
//             damp += ((punch_signal - 2048) >> 3);
//         }
//         int32_t notch = input - (bp_ * damp >> 15); 
//         lp_ += f * bp_ >> 15; 
//         CLIP(lp_)
//         int32_t hp = notch - lp_;
//         bp_ += f * hp >> 15;
//         CLIP(bp_)
//         lpOutput = (lpOutput >> 1) + lp_;
//         bpOutput = (bpOutput >> 1) + bp_;
//         hpOutput = (hpOutput >> 1) + hp;
//     }
//     CLIP(lpOutput);
//     CLIP(bpOutput);
//     CLIP(hpOutput);
//     return mode_ == BandPass ? bpOutput : (mode_ == HighPass ? hpOutput : lpOutput);
// }
}