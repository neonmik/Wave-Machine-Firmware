#include "filter.h"

namespace FILTER {
    void init() {
        lp_ = 0;
        bp_ = 0;
        frequency_ = 0 << 7;
        resonance_ = 0;
        dirty_ = true;
        punch_ = 0;
        mode_ = LowPass;
    }

    void set_frequency(uint16_t frequency) {
        frequency_ = map_exp(frequency, KNOB_MIN, KNOB_MAX, 15, 10000);
    }

    void set_resonance(uint16_t resonance) {
        resonance_ = map(resonance, KNOB_MIN, KNOB_MAX, 0, INT16_MAX);
        dirty_ = true;
    }

    void modulate_cutoff(uint16_t cutoff) {
        _mod = (cutoff >> 2);
    }

    void set_punch(uint16_t punch) {
        // punch_ = (static_cast<uint32_t>(punch) * punch) >> 24;
        punch_ = (punch >> 4);
    }

    void set_mode(uint16_t mode) {
        uint8_t index = (mode>>8);
        switch (index) {
        case 0:
            mode_ = FilterType::Off;
            break;
        case 1:
            mode_ = FilterType::LowPass;
            break;
        case 2:
            mode_ = FilterType::BandPass;
            break;
        case 3:
            mode_ = FilterType::HighPass;
            break;
        default:
            break;
        }
    }

    void process(int32_t &sample) {
        
        if (dirty_) {
            // f_ = Interpolate824(lut_svf_cutoff, frequency_ << 17);
            // f_ = frequency_;
            damp_ = Interpolate824(lut_svf_damp, resonance_ << 17);
            dirty_ = false;
        }
        int32_t f = frequency_ - _mod;
        if (f <= 15) f = 15;
        int32_t damp = damp_;
        if (punch_) {
            int32_t punch_signal = lp_ > 4096 ? lp_ : 2048;
            f += ((punch_signal >> 4) * punch_) >> 9;
            damp += ((punch_signal - 2048) >> 3);
        }

        int32_t notch = sample - (bp_ * damp >> 15);
        lp_ += f * bp_ >> 15;
        FX::HARDCLIP::process(lp_);
        // FX::SOFTCLIP::process(lp_);
        // CLIP(lp_)
        int32_t hp = notch - lp_;
        bp_ += f * hp >> 15;
        FX::HARDCLIP::process(bp_);
        // FX::SOFTCLIP::process(bp_);
        // CLIP(bp_)
        switch (mode_) {
            case Off:
                sample = sample;
                break;
            case LowPass:
                sample = lp_;
                break;
            case BandPass:
                sample = bp_;
                break;
            case HighPass:
                sample = hp;
                break;
            default:
                sample = sample;
                break;
        }
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