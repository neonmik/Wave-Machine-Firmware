#pragma once

#include "pico/stdlib.h"

long map(long x, long in_min, long in_max, long out_min, long out_max);
long map_constrained(long x, long in_min, long in_max, long out_min, long out_max);