#pragma once

#include "pico/stdlib.h"
#include "../config.h"
#include <math.h>



class LowPassFilter {
    private:
    double sampleRate;
    double cutoff;
    double prevOutput;
    double RC;
    double dt;

public:
    LowPassFilter(double cutoff) {
        this->sampleRate = SAMPLE_RATE;
        setCutoff(cutoff);
        prevOutput = 0.0;
        double RC = 1.0 / (2.0 * M_PI * cutoff);
        double dt = 1.0 / sampleRate;
    }

    void setCutoff(double cutoff) {
        if (cutoff < 0.0)
            cutoff = 0.0;
        else if (cutoff > sampleRate / 2.0)
            cutoff = sampleRate / 2.0;

        this->cutoff = cutoff;
    }

    double process(double input) {
        double inputNormalized = static_cast<double>(input) / INT16_MAX;
    
        double alpha = dt / (dt + RC);

        double outputNormalized = alpha * inputNormalized + (1.0 - alpha) * prevOutput;
        prevOutput = outputNormalized;

        return static_cast<int16_t>(outputNormalized * INT16_MAX);
    }
};

