#pragma once

#include "../config.h"

namespace FX {
    namespace SOFTCLIP {
        namespace {
            uint8_t _gain = 4;
            constexpr int16_t _threshold = INT16_MAX;
            constexpr int16_t _min = INT16_MIN; // used to be -INT16_MAX... basically 1 above the floor, dont know if that would make a difference? a hair asymetric now?
            constexpr int16_t _max = INT16_MAX;
        }
        void set_gain (uint16_t gain);
        void process (int32_t &sample);
    }
    namespace HARDCLIP {
        void process (int32_t &sample);
    }
    
}