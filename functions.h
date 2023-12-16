#pragma once

#include "pico/stdlib.h"


long map(long x, long in_min, long in_max, long out_min, long out_max);
long map_constrained(long x, long in_min, long in_max, long out_min, long out_max);

uint16_t    samples_to_us (uint32_t samplerate);

int16_t    attenuverterU10toS16 (uint16_t input);

bool getBitState(uint16_t bitField, uint16_t bitPos);
void setBit(uint16_t& bitField, uint16_t bitPos, bool value);

bool toBool(uint16_t value);
uint16_t fromBool(bool value);