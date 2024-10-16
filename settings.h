#pragma once

#include "config.h"

#include "drivers/eeprom.h"


namespace SETTINGS {

    enum LowestNote {
        C1 = 36,
        C2 = 48,
        C3 = 60,
        C4 = 72,
    };

    // padding in these structs is to keep data alignment in blocks.

    struct MidiSettingsData {
        bool        singleByteParsing               =       false;
        bool        nullVelocityTrigger             =       true;
        uint8_t     midiThruFilter                  =       0;
        uint8_t     outputNoteOffVelocity           =       0;

        uint8_t     channel;
        uint8_t     padding[3];
    };
    struct AudioSettingsData {
        uint32_t    sampleRate;
        uint8_t     padding[20];
    };
    struct SystemSettingsData {
        uint8_t     versionNumber                   =       0;
        uint8_t     ID[16];

        uint8_t     startupPreset                   =       0;
        LowestNote  startNote                       =       LowestNote::C3;

        uint32_t    globalBPM                       =       120;

        bool checkVersion (void) {
            if (versionNumber == 0) {
                printf("This is the Prototype hardware!\n");
                return true;
            } else {
                printf("Unknown Hardware version, please contact us.\n");
                return false;
            }
        }

        void assignID (void) {
            DEBUG::warning("The function 'assignID()' is not yet written");
        }

        void fetchID (void) {
            DEBUG::warning("The function 'fetchID()' is not yet written");
        }
    };

    struct PersistantData {
        MidiSettingsData midi_settings;
        AudioSettingsData audio_setting;
        SystemSettingsData system_settings;
        // uint8_t     padding[20]; // 1x 32 bit gap + 4x more 32 bit gaps...
    };

    namespace {
        PersistantData persistant_data_;
    }
    void init (void);
    void save (void);
    void load (void);
}