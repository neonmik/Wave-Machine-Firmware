#pragma once

#include "pico/stdlib.h"

namespace RANDOM {
    namespace {
        uint16_t _seed;
    }
    void update(uint16_t seed);
    uint16_t get();
};