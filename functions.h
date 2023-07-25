#pragma once

#include "pico/stdlib.h"

long map(long x, long in_min, long in_max, long out_min, long out_max);
long map_constrained(long x, long in_min, long in_max, long out_min, long out_max);
long map_exp(long x, long in_min, long in_max, long out_min, long out_max);
uint16_t exp_freq(uint16_t index);
double exponential_scaling(long x, long in_min, long in_max);
float float_log(float x);
double double_log(double x);