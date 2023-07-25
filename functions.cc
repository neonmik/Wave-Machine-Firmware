#include "functions.h"
#include "synth/log_table.h"

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
double exponential_scaling(long x, long in_min, long in_max) {
    double in_scaled = static_cast<double>(x - in_min) / static_cast<double>(in_max - in_min);
    double scaled_value = exp(in_scaled) - 1.0;
    // double scaled_value = float_log(in_scaled);
    return scaled_value;
}
