#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

// this file contains global definitions for systemwide variables and constants

constexpr   uint32_t    CORE_SPEED      =       144000;         // Core clock speed in kHz - equates to 144MHz 

constexpr   uint32_t    SAMPLE_RATE     =       48000;
constexpr   uint16_t    BUFFER_SIZE     =       64;             // Buffer size can be set at any interval (2/4/8/16/32/64/128/256)
                                                                // but the higher the size, the worst the performance

static      uint32_t    sample_clock;



constexpr   uint8_t     DEFAULT_BPM     =       120;

constexpr   uint8_t     MAX_VOICES      =       8;              // 8 is the "standard" value...
                                                                // tracking performance (Core: 144MHz/SR: 48000) :- 
                                                                // with the DAC core handling notes:
                                                                //      - runs at 10 voices with just modulation
                                                                //      - runs at 10 voices with just filter
                                                                //      - runs at 6 voices with filter and modulation mostly
                                                                // with Note Priority on HW core (Core: 144MHz/SR: 48000) :-
                                                                //      - runs at 8 voices with both

                                                    
constexpr   uint8_t     MIDI_CHANNEL    =       0;              // 0 - 15 available

constexpr   uint8_t     DEFAULT_KEY     =       48;

constexpr   uint16_t    KNOB_MIN        =       0;
constexpr   uint16_t    KNOB_MAX        =       1023;

enum                    LFO_SPEED {                             // Hz = 1 (cycles) / seconds per full cycle
                                        PAINFUL = 16,           // 0.00286Hz -   38.46Hz (5:49:497  - 0:00:0260)
                                        SLOWER  = 15,           // 0.00572Hz -   76.92Hz (5:49:497  - 0:00:0130)
                                        SLOW    = 14,           // 0.01144Hz -  153.84Hz (1:27:363  - 0:00:0065)
                                        NORMAL  = 13,           // 0.02289Hz -  307.69Hz (0:43:628  - 0:00:00325)
                                        FAST    = 12,           // 0.04578Hz -  615.38Hz (0:21:814  - 0:00:001625)
                                        FASTER  = 11,           // 0.09156Hz - 1230.77Hz (0:10:907  - 0:00:0008125)
                                        SILLY   = 10            // 0.18312Hz - 2461.54Hz (0:05.453  - 0:00:00040625)
};

