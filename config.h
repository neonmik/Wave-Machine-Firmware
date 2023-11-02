#pragma once

#include "pico/stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

#include "version.h"

#include "debug.h"
#include "functions.h"

// this file contains global definitions for systemwide variables and constants

// -------------------------------
//        Software Defines
// -------------------------------

constexpr   uint8_t     POLYPHONY       =       8;              // 8 is the "standard" value...
                                                                // tracking performance (Core: 144MHz/SR: 48000) :- 
                                                                // with the DAC core handling notes:
                                                                //      - runs at 10 voices with just modulation
                                                                //      - runs at 10 voices with just filter
                                                                //      - runs at 6 voices with filter and modulation mostly
                                                                // with Note Priority on HW core (Core: 144MHz/SR: 48000) :-
                                                                //      - runs at 8 voices with both
                                                                // with Synth code being handled outside of the DMA (Core: 144MHz/SR: 48000) :-
                                                                //      - runs at 12 with both - maybe try adding a second oscillator?
                                                                //      - runs at 10 with Arp going full speed.
                                                                //      - runs at 8 with the rough second oscillator !!!ROUGH!!!

constexpr   uint8_t     MAX_ARP         =       POLYPHONY;      // For setting the maximum number of notes in the Arp
                                                    
constexpr   uint8_t     MIDI_CHANNEL    =       0;              // 0 - 15 available
constexpr   uint8_t     MIDI_DEFAULT_NOTE_OFF_VEL = 0;

// -------------------------------
//        Hardware Defines
// -------------------------------
// These are the settings which are used to define the hardware interfaces

constexpr   uint32_t    CORE_SPEED      =       144000;         // Core clock speed in kHz - equates to 144MHz 

constexpr   uint16_t    SAMPLE_RATE     =       32000;          // Default is 32000Hz currently... 44100Hz works, but is stressed at crazier settings.
constexpr   uint16_t    NYQUIST         =       SAMPLE_RATE/2;  // Easiest way of deliniating Nyquist when changing Sample Rate in protoype stages.
constexpr   uint16_t    BUFFER_SIZE     =       16;             // The DMA buffer size can be set at any interval (2/4/8/16/32/64/128/256)

constexpr   uint8_t     MAX_PAGES       =       4;
constexpr   uint8_t     MAX_KNOBS       =       4;

constexpr   uint8_t     MAX_PRESETS     =       8;

constexpr   uint16_t    KNOB_MIN        =       0;
constexpr   uint16_t    KNOB_MAX        =       1023;

// -------------------------------
//      Wave machine defaults
// -------------------------------
// These are the settings that are assigned on startup, before any user settings are loaded from the EEPROM.

constexpr   uint8_t     DEFAULT_BPM     =       120;            // The default BPM of the system

constexpr   uint8_t     DEFAULT_KEY     =       60;             // The default first key on the keyboard (C3 == 60, C4 == 72, etc). 
                                                                // 48 sets C3 as the lowest note one the keyboard, and the middle C on the keyboard as C4
constexpr   uint8_t     DEFAULT_ON_VEL  =       127;            // The default velocity of the inbuilt keys
constexpr   uint8_t     DEFAULT_OFF_VEL =       0;              // The default velocity of the inbuilt keys



constexpr   uint8_t     DEFAULT_PRESET  =       0;


// -------------------------------
//        Global Variables
// ------------------------------- 
// These are variables that are used globaly

extern      uint32_t    sampleClock;
extern      uint8_t     softwareIndex;
extern      uint8_t     hardwareIndex;
extern      uint16_t    playBuffer[];


// example struct of the settings struct

// namespace CONFIG {
//     struct SETTINGS {
//         struct HARDWARE {
//             char ID[16];

//         };
        
//     }
// }