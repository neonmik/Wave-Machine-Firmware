#include "functions.h"

#include <math.h>

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
long map_constrained(long x, long in_min, long in_max, long out_min, long out_max) {
    long temp = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        if (temp > out_max) temp = out_max;
        if (temp < out_min) temp = out_min;
        return temp;
}

inline uint16_t Interpolate824(const uint16_t* table, uint32_t phase) {
    uint32_t a = table[phase >> 24];
    uint32_t b = table[(phase >> 24) + 1];
    return a + ((b - a) * static_cast<uint32_t>((phase >> 8) & 0xffff) >> 16);
}

inline int16_t Interpolate824(const uint8_t* table, uint32_t phase) {
    int32_t a = table[phase >> 24];
    int32_t b = table[(phase >> 24) + 1];
    return (a << 8) + \
        ((b - a) * static_cast<int32_t>(phase & 0xffffff) >> 16) - 32768;
}

uint16_t    samples_to_us (uint32_t samplerate) {
    return (1000000 / samplerate);
}

int16_t    attenuverter (uint16_t input) {
    // printf("Attenuverter not written yet! See function header.\n");
    return input;
}