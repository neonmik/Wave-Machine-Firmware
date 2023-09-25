#include "settings.h"

namespace SETTINGS {
    void init() {
        persistant_data_.midi_settings.channel = MIDI_CHANNEL;

        persistant_data_.audio_setting.sample_rate = SAMPLE_RATE;

        persistant_data_.system_settings.bpm = DEFAULT_BPM;
        
    }
}