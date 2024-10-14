// ----------------------
//          DAC
// ----------------------

#include "dac.h"

uint32_t    sampleClock;
uint8_t     softwareIndex = 0;
uint8_t     hardwareIndex = 2;
volatile uint16_t    playBuffer[256];

namespace DAC {

    void init (void) {
        _clock_speed = clock_get_hz(clk_sys);
        _divider = _clock_speed / SAMPLE_RATE;

        init_spi();
        init_dma();
    }
}