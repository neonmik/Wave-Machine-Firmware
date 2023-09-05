// ----------------------
//          DAC
// ----------------------

#include "dac.h"

uint32_t    sample_clock;
uint8_t     softwareIndex = 0;
uint8_t     hardwareIndex = 2;
uint16_t    playBuffer[256];

namespace DAC {

    void Init (synth_function audio_process) {
        process = audio_process;

        _clock_speed = clock_get_hz(clk_sys);
        _divider = _clock_speed / SAMPLE_RATE;

        init_spi();
        init_dma();
    }
    void clear_state (void) {
        _full = false;
    }
    bool get_state (void) {
        return _full;
    }
}