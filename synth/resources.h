#pragma once

#include "pico/stdlib.h"

extern const uint16_t lut_exp_freq[];
extern const uint16_t lut_filter_damp[];
extern const uint16_t lut_pitch_log[];

uint16_t exp_freq(uint16_t index);
uint16_t filter_damp(uint16_t index);
uint16_t pitch_log(uint16_t index);