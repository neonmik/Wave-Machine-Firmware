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
long map_exp(long x, long in_min, long in_max, long out_min, long out_max) {
    double scaled_value = exponential_scaling(x, in_min, in_max);
    return map(static_cast<long>(scaled_value * 1023), 0, 1023, out_min, out_max);
}
double exponential_scaling(long x, long in_min, long in_max) {
    double in_scaled = static_cast<double>(x - in_min) / static_cast<double>(in_max - in_min);
    double scaled_value = exp(in_scaled) - 1.0;
    return scaled_value;
}
// double exponential_scaling(long x, long in_min, long in_max) {
//     double in_scaled = static_cast<double>(x - in_min) / static_cast<double>(in_max - in_min);
//     double scaled_value = pow(in_scaled, 0.5);
//     return scaled_value;
// }
// long map_exp(long x, long in_min, long in_max, long out_min, long out_max) {
//     double log_min = log10(out_min);
//     double log_max = log10(out_max);
//     double scaled_value = map(static_cast<double>(x), in_min, in_max, log_min, log_max);
//     double frequency = pow(10, scaled_value);
//     return frequency;
// }