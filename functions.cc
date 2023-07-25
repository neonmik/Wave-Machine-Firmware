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
long map_exp(long x, long in_min, long in_max, long out_min, long out_max) {

    // double scaled_value = exponential_scaling(x, in_min, in_max);
    double scaled_value = double_log((double)x / 1023.0);
    return map(static_cast<long>(scaled_value * 1023), 0, 1023, out_min, out_max);
}
uint16_t exp_freq(uint16_t index) {
    return exp_freq_lut[index];
}
double exponential_scaling(long x, long in_min, long in_max) {
    double in_scaled = static_cast<double>(x - in_min) / static_cast<double>(in_max - in_min);
    double scaled_value = exp(in_scaled) - 1.0;
    // double scaled_value = float_log(in_scaled);
    return scaled_value;
}
float float_log(float x) {

    float y = x * 0.25;

    if (x > 0.5) {
        y = x * 2 - y - 0.75;
    }

    return y;
}
double double_log(double x) {
    double y = x * 0.25;

    if (x > 0.5) {
        y = x * 2 - y - 0.75;
    }

    return y;
}
// uint16_t log_map (uint16_t input, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    // uin16_t output = input >> 2;

    // if (input > (UINT_MAX >> 1)) {
    //     output = input << 1 - output -  ((UINT_MAX >> 1) + (UINT_MAX >> 2));
    // }

    // return output;
// }