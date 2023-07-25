#include "filter.h"

namespace FILTER {

    ADSREnvelope ADSR{_attack, _decay, _sustain, _release};

    void init() {
        lowpass_ = 0;
        bandpass_ = 0;
        cutoff_ = 0 << 7;
        resonance_ = 0;
        dirty_ = true;
        punch_ = 0;
        mode_ = LowPass;
    }

    void set_cutoff(uint16_t cutoff) {
        cutoff_ = exp_freq(cutoff);
    }

    void set_resonance(uint16_t resonance) {
        resonance_ = map(resonance, KNOB_MIN, KNOB_MAX, 0, INT16_MAX);
        dirty_ = true;
    }

    void modulate_cutoff(uint16_t cutoff) {
        mod_ = (cutoff >> 2);
    }

    void set_punch(uint16_t punch) {
        // punch_ = (static_cast<uint32_t>(punch) * punch) >> 24;
        punch_ = (punch);
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

    void set_attack (uint16_t attack) {
        if (attack == _last_attack) return;
        _last_attack = attack;
        _attack = calc_end_frame((attack<<2)+2);
    }
    void set_decay (uint16_t decay) {
        if (decay == _last_decay) return;
        _last_decay = decay;
        _decay = calc_end_frame((decay<<2)+2);
    }
    void set_sustain (uint16_t sustain) {
        if (sustain == _last_sustain) return;
        _last_sustain = sustain;
        _sustain = (sustain << 6);
    }
    void set_release (uint16_t release) {
        if (release == _last_release) return;
        _last_release = release;
        _release = calc_end_frame((release<<2)+2);
    }

    void trigger_attack (void) {
        ADSR.trigger_attack();
    }
    void trigger_release (void) {
        ADSR.trigger_release();
    }

    void process(int32_t &sample) {
        ADSR.update();
        // if (ADSR.isStopped()) return;
        
        // dirty is/was for taking a simple input number and using a lookup table to calculate a smooth frequency input.
        if (dirty_) {
            // f_ = Interpolate824(lut_svf_cutoff, frequency_ << 17);
            // f_ = frequency_;
            damp_ = Interpolate824(lut_svf_damp, resonance_ << 17);
            dirty_ = false;
        }


        volatile int32_t _frequency;
        _frequency = (int32_t(cutoff_) * int32_t(ADSR.get_adsr() >> 8)) >> 16;
        
        // if (f <= 15) f = 15;
        int32_t damp = damp_;
        if (punch_) {
            int32_t punch_signal = lowpass_ > 4096 ? lowpass_ : 2048;
            _frequency += ((punch_signal >> 4) * punch_) >> 9;
            damp += ((punch_signal - 2048) >> 3);
        }

        int32_t notch = sample - (bandpass_ * damp >> 15);
        lowpass_ += _frequency * bandpass_ >> 15;
        FX::HARDCLIP::process(lowpass_);
        // FX::SOFTCLIP::process(lowpass_); // this makes the signal super hot... not sure why.
        
        int32_t highpass_ = notch - lowpass_;
        bandpass_ += _frequency * highpass_ >> 15;
        FX::HARDCLIP::process(bandpass_);
        // FX::SOFTCLIP::process(bandpass_); // this makes the signal super hot... not sure why.

        switch (mode_) {
            case Off:
                sample = sample;
                break;
            case LowPass:
                sample = lowpass_;
                break;
            case BandPass:
                sample = bandpass_;
                break;
            case HighPass:
                sample = highpass_;
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
//             int32_t punch_signal = lowpass_ > 4096 ? lowpass_ : 2048;
//             f += ((punch_signal >> 4) * punch_) >> 9;
//             damp += ((punch_signal - 2048) >> 3);
//         }
//         int32_t notch = input - (bandpass_ * damp >> 15); 
//         lowpass_ += f * bandpass_ >> 15; 
//         CLIP(lowpass_)
//         int32_t hp = notch - lowpass_;
//         bandpass_ += f * hp >> 15;
//         CLIP(bandpass_)
//         lpOutput = (lpOutput >> 1) + lowpass_;
//         bpOutput = (bpOutput >> 1) + bandpass_;
//         hpOutput = (hpOutput >> 1) + hp;
//     }
//     CLIP(lpOutput);
//     CLIP(bpOutput);
//     CLIP(hpOutput);
//     return mode_ == BandPass ? bpOutput : (mode_ == HighPass ? hpOutput : lpOutput);
// }
}