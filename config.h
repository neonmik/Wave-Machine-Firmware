#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

// this file contains global definitions for systemwide variables and constants

constexpr   uint32_t    CORE_SPEED      =       144000;

constexpr   uint32_t    SAMPLE_RATE     =       48000;
constexpr   uint8_t     MAX_VOICES      =       8; // 8 is the "standard" value, it can currently (in release mode) run up to 15. Crashes on 16. 
                                                    // going to use the processing power from the "spare" voices for 
constexpr   uint8_t     DEFAULT_BPM     =       120;

constexpr   uint8_t     MIDI_CHANNEL    =       0;
constexpr   uint16_t    KNOB_MIN        =       0;
constexpr   uint16_t    KNOB_MAX        =       1023;

static      uint32_t    sample_clock;

struct MidiSettings {
    uint8_t     channel;
};