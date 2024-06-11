#pragma once

#include "../config.h"

extern const uint16_t lut_exp_freq[];
extern const uint16_t lut_filter_damp[];
extern const uint16_t lut_pitch_log[];
extern const uint32_t note2freq[];


uint32_t getFrequency(uint8_t note);
uint16_t exponentialFrequency(uint16_t index);
uint16_t filterDamp(uint16_t index);
uint16_t logarithmicPitch(uint16_t index);
uint16_t logPotentiometer(uint16_t index);

extern const uint8_t hexValues[];