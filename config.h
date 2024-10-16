#pragma once

#include "pico/stdlib.h"
#include "pico/unique_id.h"

#include "hardware/spi.h"
#include "hardware/i2c.h"

#include "stdio.h"
#include "string.h"
#include "math.h"

#include "version.h"

#include "debug.h"
#include "functions.h"
#include "synth/resources.h"

// this file contains global definitions for systemwide variables and constants

// -------------------------------
//        Software Defines
// -------------------------------

constexpr   uint8_t     POLYPHONY       =       8;              // 8 is the "standard" value - fine @ 44.1kHz
                                                                // 10 is the "max" value - fine @ 44.1kHz 


constexpr   uint8_t     MAX_ARP         =       32;             // For setting the maximum number of notes in the Arp
                                                    
constexpr   uint8_t     MIDI_CHANNEL    =       0;              // 0 - 15 available
constexpr   uint8_t     MIDI_DEFAULT_NOTE_OFF_VEL = 0;

constexpr   uint8_t     MAX_PAGES       =       4;
constexpr   uint8_t     MAX_KNOBS       =       4;

constexpr   uint8_t     MAX_PRESETS     =       8;

////////////
// EEPROM //
////////////

constexpr   uint8_t         ADDRESS_SIZE            =       2;                  // The size of the EEPROM address in bytes
constexpr   uint8_t         PAGE_SIZE               =       64;                 // The size of a Page in the EEPROM (64 bytes)

// TODO: Remove these once EEPROM is cleared -----------
constexpr   uint8_t         OLD_PRESET_SIZE             =       (PAGE_SIZE * 2);    // The size of a Preset (currently 64 bytes, but may become more in future, hence 128 bytes per preset)

constexpr   uint16_t        OLD_PRESET_ADDRESS          =       0x0;                // Pages 0   - 127 are for Presets
constexpr   uint16_t        OLD_FACTORY_PRESET_ADDRESS  =       0x2000;             // Pages 128 - 255 are for Factory Presets
constexpr   uint16_t        OLD_FREE_ADDRESS            =       0x4000;             // Pages 256 - 508 are empty for now
// -----------------------------------------------------

constexpr   uint16_t        PRESET_SIZE             =       (PAGE_SIZE * 3);    // The size of a Preset is 192 bytes
constexpr   uint16_t        PRESET_ADDRESS          =       0x0;                // Pages 0    - 191 are for Presets (8 Banks of 8 Presets)
constexpr   uint16_t        FACTORY_PRESET_ADDRESS  =       0x3000;             // Pages 192  - 383 are for Factory Presets
constexpr   uint16_t        FREE_ADDRESS            =       0x6000;             // Pages 384  - 508 are empty for now

constexpr   uint16_t        SYSTEM_SETTINGS_ADDRESS =       0x7EFF;             // Pages 508 - 511 are for System Settings
constexpr   uint16_t        MAX_ADDRESS             =       0x7FFF;             // The last readable address in the EEPROM

/////////////
// BUTTONS //
/////////////

constexpr   uint16_t    SHIFT_TIMEOUT       =       85;             // The time in ms before the shift button is considered to be held down 
constexpr   uint16_t    DEBOUNCE_TIME       =       5;
constexpr   uint16_t    SHORT_PRESS_TIME    =       192;
constexpr   uint16_t    DOUBLE_PRESS_TIME   =       150;
constexpr   uint16_t    LONG_PRESS_TIME     =       350;
constexpr   uint16_t    TOTAL_TIMEOUT       =       650;
constexpr   uint16_t    PROTECTION_THRESHOLD =      5;            // The number of steps a knob must be turned before it is considered to be a "real" change

//////////
// LEDS //
//////////

enum LED_SPEED : uint8_t {
    FAST            = 4,
    NORMAL          = 8, 
    SLOW            = 48, // probably should be used for constant flashing
};
 


/////////
// ADC //
/////////

constexpr   uint16_t    KNOB_MIN        =       0;
constexpr   uint16_t    KNOB_MAX        =       1023;
// these are set inside the range of the ADC to compensate for the noise floor (on the minimum) and battery power (on the maximum)
constexpr   uint16_t    INPUT_RANGE_MIN = 10;
constexpr   uint16_t    INPUT_RANGE_MAX = 3992;

constexpr   uint8_t     MAX_NOISE_READINGS = 32;

constexpr   uint8_t     HYSTERESIS_WEIGHTING = 5;
constexpr   uint8_t     IIR_FILTER_WEIGHTING = 6;

constexpr   float       MINIMUM_BATTERY_VOLTAGE = 3.58;

/////////
// DAC //
/////////

constexpr   uint8_t     PRESET_VERSION  =       1;

// -------------------------------
//        Hardware Defines
// -------------------------------
// These are the settings which are used to define the hardware interfaces

constexpr   uint32_t    CORE_SPEED      =       150000;         // Core clock speed in kHz - equates to 150MHz (default core speed on the RP2350, this is here for other variables)

/////////
// DAC //
/////////

constexpr   uint32_t    SAMPLE_RATE     =       44100;          
constexpr   uint16_t    BUFFER_SIZE     =       16;             // The DMA buffer size can be set at any interval (2/4/8/16/32/64/128/256)

constexpr   uint8_t     DAC_DATA    =   11;
constexpr   uint8_t     DAC_CLK     =   10;
constexpr   uint8_t     DAC_CS      =   9;
#define                 DAC_SPI         spi1

constexpr   uint16_t    DAC_CONFIG  =   0b0111000000000000;

constexpr   uint32_t    NYQUIST         =       SAMPLE_RATE/2;  // Easiest way of deliniating Nyquist when changing Sample Rate in protoype stages.

////////////
// EEPROM //
////////////
constexpr   uint8_t         EEPROM_SDA_PIN          =       2;
constexpr   uint8_t         EEPROM_SCL_PIN          =       3;

constexpr   i2c_inst_t*     EEPROM_I2C_CHANNEL      =       i2c1;
constexpr   uint8_t         EEPROM_I2C_ADDRESS      =       0x50;

/////////////
// BUTTONS //
/////////////

constexpr   uint8_t     MUX_OUT_0   =   16;
constexpr   uint8_t     MUX_OUT_1   =   17;

constexpr   uint8_t     MAX_KEYS    =   27;
constexpr   uint8_t     PAGE_KEY    =   27;
constexpr   uint8_t     LFO_KEY     =   28;
constexpr   uint8_t     ARP_KEY     =   29;
constexpr   uint8_t     PRESET_KEY  =   30;

/////////
// ADC //
/////////

enum ADC_GPIO : uint8_t {
    ADC_MUX_PIN         = 26,
    ADC_NOISE_PIN       = 27,
    ADC_VOLTAGE_PIN     = 43,
};

enum ADC_CHANNEL : uint8_t {
    ADC_MUX_CHANNEL         = 0,
    ADC_NOISE_CHANNEL       = 1,
    ADC_VOLTAGE_CHANNEL     = 3,
    ADC_TEMPERATURE_CHANNEL = 8
};

//////////
// LEDS //
//////////

enum LED_PINS : uint8_t {
    PICO            = PICO_DEFAULT_LED_PIN, // 25
    FUNC1           = 21,
    FUNC2           = 22,
};

constexpr   uint8_t     LEDR_PIN =                  6;
constexpr   uint8_t     LEDG_PIN =                  7;
constexpr   uint8_t     LEDB_PIN =                  8;  

constexpr   uint16_t    PWM_BIT_DEPTH =             255; // or 65535   
constexpr   uint16_t    PWM_HZ =                    48000;
constexpr   float       PWM_DIV =                   ((CORE_SPEED * 1000) / PWM_BIT_DEPTH) / SAMPLE_RATE;

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
extern      volatile uint16_t    playBuffer[]; //TODO: #27 look to replace this with a static variable... should just be the one buffer instance, and can be decalred in functions that need it at init.

extern      bool        startUpComplete;

static      uint8_t     hardwareID[PICO_UNIQUE_BOARD_ID_SIZE_BYTES];