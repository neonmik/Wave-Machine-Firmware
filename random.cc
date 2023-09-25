#include "random.h"

namespace RANDOM {
    void update(uint16_t seed) {
        // update the seed with the input (called from ADC)
        _seed += seed;
        // Perform some simple bitwise operations on the seed to generate a pseudo-random number
        _seed ^= (_seed << 7);
        _seed ^= (_seed >> 9);
        _seed ^= (_seed << 8);
    }

    uint16_t get() {
        return _seed;
    }


    // old PRNG functions, may be useful to base a normalised noise calc off of
    // uint32_t prng_xorshift_state = 0x32B71700;

    // uint32_t prng_xorshift_next() {
    //   uint32_t x = prng_xorshift_state;
    //   x ^= x << 13;
    //   x ^= x >> 17;
    //   x ^= x << 5;
    //   prng_xorshift_state = x;
    //   return x;
    // }
    // int32_t prng_normal() {
    //     // rough approximation of a normal distribution
    //     uint32_t r0 = prng_xorshift_next();
    //     uint32_t r1 = prng_xorshift_next();
    //     uint32_t n = ((r0 & 0xffff) + (r1 & 0xffff) + (r0 >> 16) + (r1 >> 16)) / 2;
    //     return n - 0xffff;
    // }
}