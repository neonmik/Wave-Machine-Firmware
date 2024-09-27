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
long constrain (long x, long min, long max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
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

int16_t    attenuverterU10toS16 (uint16_t input) {
    // takes an unsigned 10 bit value and returns a signed 16 bit value
    return (static_cast<int16_t>(input) - 512) << 6;
}
int16_t    attenuverterU10 (uint16_t input) {
    // takes an unsigned 10bit int and returns a signed 10bit int
    return (static_cast<int16_t>(input) - 512);
}

bool getBitState(uint16_t bitField, uint16_t bitPos) {
    return (bitField & (1 << bitPos)) != 0;
}
void setBit(uint16_t& bitField, uint16_t bitPos, bool value) {
    if (value) {
        // Set the bit
        bitField |= (1 << bitPos);
    } else {
        // Clear the bit
        bitField &= ~(1 << bitPos);
    }
}

bool toBool(uint16_t value) {
    // Consider values greater than or equal to 512 as true
    return value >= 512;
}
uint16_t fromBool(bool value) {
    // Map true to 1023 and false to 0
    return value ? 1023 : 0;
}
void printHexBuffer(const uint8_t* buffer, size_t length, size_t lineBreak) {
    for (size_t i = 0; i < length; i++) {
        printf("0x%02X ", buffer[i]);
        if ((i + 1) % lineBreak == 0) {
            printf("\n");
        }
    }
}

void applyHysteresis(int& value, int new_value, int hysteresis) {
    if (abs(new_value - value) > hysteresis) {
        value = new_value;
    }
}

