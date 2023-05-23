#pragma once

#include "pico/stdlib.h"

#include "../mailbox.h"
#include "../config.h"

#include "arp.h"


enum class Priority {
    FIRST,      // First/oldest notes stay the longest
    LAST,       // Last/newest notes stay the longest
    HIGHEST,    // Highest notes stay the longest
    LOWEST      // Lowest notes stay the longest
};

namespace NOTE_PRIORITY {


    namespace {
        MAILBOX::note_data& NOTES = MAILBOX::NOTE_DATA.core0;

        uint16_t note2freq[] = {8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};
        inline uint16_t get_freq(uint8_t note) {
            return note2freq[note];
        }
    
        Priority    _priority = Priority::LAST;

        // uint8_t     _note_state[128];
        uint8_t     _note_state_last[128];
        
        uint8_t     _voice_notes[MAX_VOICES];
        uint32_t    _time_activated[MAX_VOICES];
        bool        _active_voice[MAX_VOICES];
        bool        _released_voice[MAX_VOICES];

        int8_t     _voices_active = 0;
    }
    
    // actual synth voice notes, also add MIDI out here
    void voice_on(int slot, int note, int velocity);
    void voice_off(int slot, int note, int velocity);
    void voices_panic(void);

    // Note priority detection
    void priority(int status, int note, int velocity);

    void update (void);
}

