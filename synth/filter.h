#pragma once

#include "pico/stdlib.h"
#include <math.h>



class LowPassFilter {
    public:
    LowPassFilter(int32_t cutoffFrequency, int32_t sampleRate) {
        // Convert cutoff frequency and sample rate to fixed-point representation
        int64_t cutoffFrequencyFixed = static_cast<int64_t>(cutoffFrequency) << kQ;
        int64_t sampleRateFixed = static_cast<int64_t>(sampleRate) << kQ;

        // Calculate filter coefficient in fixed-point representation
        alpha_ = static_cast<int32_t>((2LL * M_PI * cutoffFrequencyFixed) / sampleRateFixed);
        // alpha_ = static_cast<int32_t>((6.283185307179586476925286766559 * cutoffFrequencyFixed) / sampleRateFixed);


        output_ = 0;
    }

    int32_t process(int32_t input) {
        // Perform fixed-point multiplication and division
        volatile int64_t inputFixed = static_cast<int64_t>(input) << kQ;
        volatile int64_t outputFixed = (alpha_ * (inputFixed - output_)) >> kQ;
        output_ += static_cast<int32_t>(outputFixed);

        // Perform saturation to prevent overflow
        if (output_ > kMaxOutput) {
            output_ = kMaxOutput;
        } else if (output_ < kMinOutput) {
            output_ = kMinOutput;
        }

        return output_;
    }

    private:
    static constexpr int32_t kQ = 20; // Number of fractional bits
    static constexpr int32_t kMaxOutput = INT32_MAX; // Maximum output value
    static constexpr int32_t kMinOutput = INT32_MIN; // Minimum output value
    int32_t alpha_;
    int32_t output_;
};

