#include "random.h"

namespace RANDOM {
    void update(uint16_t seed) {
        // Update the seed with the input (called from ADC)
        _seed += seed;
        // Perform some simple bitwise operations on the seed to generate a pseudo-random number
        _seed ^= (_seed << 7);
        _seed ^= (_seed >> 9);
        _seed ^= (_seed << 8);
    }

    uint16_t get() {
        return _seed;
    }
}