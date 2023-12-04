#include "settings.h"

namespace SETTINGS {
    void init() {
        persistant_data_.midi_settings.channel = MIDI_CHANNEL;

        persistant_data_.audio_setting.sampleRate = SAMPLE_RATE;

        persistant_data_.system_settings.globalBPM = DEFAULT_BPM;

        // eventually this will just load from EEPROM
    }
}