#pragma once

#include "pico/stdlib.h"
#include "stdio.h"



long map(long x, long in_min, long in_max, long out_min, long out_max);
long map_constrained(long x, long in_min, long in_max, long out_min, long out_max);
long constrain (long x, long min, long max);

uint16_t    samples_to_us (uint32_t samplerate);

inline int16_t    attenuverterU10toS9 (uint16_t input) {
    // takes an unsigned 10bit int and returns a signed 10bit int
    return (static_cast<int16_t>(input) - 512);
}

inline int16_t    attenuverterU10toS10 (int16_t input) {
    int16_t temp = static_cast<int16_t>(input) - 512;
    return temp << 1;
}

bool getBitState(uint16_t bitField, uint16_t bitPos);
void setBit(uint16_t& bitField, uint16_t bitPos, bool value);

bool toBool(uint16_t value);
uint16_t fromBool(bool value);

void printHexBuffer(const uint8_t* buffer, size_t length, size_t lineBreak = 4);

void applyHysteresis(int& value, int new_value, int hysteresis);

inline void tickLoop (uint32_t& tick, uint32_t interval) {
    ++tick;
    tick %= interval;
}
inline void tickLoop (uint16_t& tick, uint16_t interval) {
    ++tick;
    tick %= interval;
}
inline void tickLoop (uint8_t& tick, uint8_t interval) {
    ++tick;
    tick %= interval;
}