#pragma once

#include "pico/stdlib.h"

#include "config.h"

#include "drivers/eeprom.h"


namespace SETTINGS {

    // padding in these structs is to keep data alignment in blocks.

    struct MidiSettingsData {
        uint8_t     channel;
        uint8_t     padding[3];
    };
    struct AudioSettingsData {
        uint32_t    sample_rate;
    };
    struct SystemSettingsData {
        uint32_t    bpm;
    };

    struct PersistantData {
        MidiSettingsData midi_settings;
        AudioSettingsData audio_setting;
        SystemSettingsData system_settings;
        uint8_t     padding[20]; // 1x 32 bit gap + 4x more 32 bit gaps...
    };

    namespace {
        PersistantData persistant_data_;
    }
    void Init (void);
    void save (void);
    void load (void);
}