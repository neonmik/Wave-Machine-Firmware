#pragma once

#include "../config.h"

namespace FX {
    namespace SOFTCLIP {
        namespace {
            uint8_t _gain = 1;
            constexpr int16_t _threshold = INT16_MAX;
            constexpr int16_t _min = INT16_MIN;
            constexpr int16_t _max = INT16_MAX;
        }
        void setGain (uint16_t gain);
        void process (int32_t &sample);
    }
    namespace HARDCLIP {
        void process16 (int32_t &sample);
        void process (int32_t &sample);
    }
    
}