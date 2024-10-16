#pragma once

#include "pico/stdlib.h"


namespace INTERPOLATION {
    void init (void);
    int16_t process(int32_t frac, int16_t sample1, int16_t sample2);
}