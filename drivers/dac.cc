// ----------------------
//          DAC
// ----------------------

#include "dac.h"

namespace DAC {
    void init (uint16_t sample_rate) {
        _sample_rate = sample_rate;

        _clock_speed = clock_get_hz(clk_sys);
        _divider = _clock_speed / _sample_rate;

        init_spi(_sample_rate);
        init_dma();
    }
    void fill (uint16_t buffer, uint16_t index) {
        _buffer[index] = buffer;
    }
    void clear_state (void) {
        _full = false;
    }
    bool get_state (void) {
        return _full;
    }
}