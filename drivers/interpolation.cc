#include "interpolation.h"

#include "hardware/interp.h"

namespace INTERPOLATION {
    void init (void) {
        const int uv_fractional_bits = 12;

        // Lane 0 setup: fractional interpolation
        interp_config cfg = interp_default_config();
        interp_config_set_shift(&cfg, uv_fractional_bits - 1);  // Shift by 11 bits (12-bit fractional part)
        interp_config_set_mask(&cfg, 1, 32 - uv_fractional_bits);  // Mask for fractional bits
        interp_config_set_blend(&cfg, true);  // Enable blending
        interp_set_config(interp0, 0, &cfg);

        // Lane 1 setup: signed interpolation with cross-input
        cfg = interp_default_config();
        interp_config_set_shift(&cfg, uv_fractional_bits - 8);  // Shift to handle blending across lanes
        interp_config_set_signed(&cfg, true);  // Enable signed arithmetic
        interp_config_set_cross_input(&cfg, true);  // Cross-input to blend with lane 0
        interp_set_config(interp0, 1, &cfg);
    }

    int16_t process(int32_t frac, int16_t sample1, int16_t sample2) {
        // Set the fractional part and wavetable samples for interpolation
        interp0->accum[0] = frac;    // Load fractional part into accum[0]
        interp0->base[0] = sample1;  // Load sample1 into base 0
        interp0->base[1] = sample2;  // Load sample2 into base 1

        // Retrieve the interpolated result from the hardware
        return interp0->peek[1];  // Final interpolated value from lane 1
    }
    
}