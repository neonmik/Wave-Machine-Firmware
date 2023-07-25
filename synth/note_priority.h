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

        // uint16_t note2freq[] = {8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};
        // inline uint16_t get_freq(uint8_t note) {
        //     return note2freq[note];
        // }

        const uint32_t note2freq[] = {
            535809,	    567670,	    601425,	    637188,	    675077,	    715219,	    757748,	    802806,
            850544,	    901120,	    954703,	    1011473,	1071618,	1135340,	1202850,	1274376,
            1350154,	1430438,	1515497,	1605613,	1701088,	1802240,	1909406,	2022946,
            2143236,	2270680,	2405701,	2548752,	2700308,	2860877,	3030994,	3211226,
            3402176,	3604480,	3818813,	4045892,	4286473,	4541360,	4811403,	5097504,
            5400617,	5721755,	6061988,	6422453,	6804352,	7208960,	7637627,	8091784,
            8572946,	9082720,	9622807,	10195009,	10801235,	11443510,	12123977,	12844906,
            13608704,	14417920,	15275254,	16183568,	17145893,	18165440,	19245614,	20390018,
            21602471,	22887021,	24247954,	25689812,	27217408,	28835840,	30550508,	32367136,
            34291786,	36330881,	38491228,	40780036,	43204943,	45774042,	48495908,	51379625,
            54434817,	57671680,	61101016,	64734272,	68583572,	72661763,	76982456,	81560072,
            86409886,	91548085,	96991817,	102759251,	108869634,	115343360,	122202033,	129468544,
            137167144,	145323527,	153964913,	163120144,	172819772,	183096171,	193983635,	205518503,
            217739269,	230686720,	244404066,	258937088,	274334288,	290647054,	307929827,	326240288,
            345639545,	366192342,	387967271,	411037006,	435478538,	461373440,	488808132,	517874176,
            548668577,	581294108,	615859655,	652480576,	691279090,	732384684,	775934543,	822074012,
        };
        inline uint32_t get_freq(uint8_t note) {
            return note2freq[note];
        }

    
        Priority    _priority = Priority::LAST;

        uint8_t     _notes_on;
        uint8_t     _note_state[128];
        uint8_t     _note_state_last[128];
        
        uint8_t     _voice_notes[MAX_VOICES];
        uint32_t    _time_activated[MAX_VOICES];
        bool        _active_voice[MAX_VOICES];
        bool        _released_voice[MAX_VOICES];

        int8_t     _voices_active = 0;

        void        voices_inc (void) {
            _voices_active++;
            if (_voices_active > 8) {
                _voices_active = 8;
            }
        }
        void        voices_dec (void) {
            --_voices_active;
            if (_voices_active <= 0) {
                _voices_active = 0;
            }
        }
        bool        voices_active (void) {
            return _voices_active;
        }

        struct voice_data {
            uint8_t     note; // holds MIDI note number
            bool        gate; // tracks whether note is being physcially played
            bool        active; // tracks whether the note is still sounding out
            uint32_t    activation_time; // time when the note was activated

            void voice_on (uint8_t note, uint8_t velocity) {
                note = note;
                gate = true;
                active = true;
                activation_time = to_ms_since_boot(get_absolute_time());
            }
            void voice_off (uint8_t note, uint8_t velocity) {
                note = 0;
                gate = false;
            }
        };
    }
    
    extern voice_data VOICES[8];
    // actual synth voice notes, also add MIDI out here
    void voice_on(int slot, int note, int velocity);
    void voice_off(int slot, int note, int velocity);
    void voices_panic(void);

    // filter env
    void filter_on (void);
    void filter_off (void);

    // Note priority detection
    void priority(int status, int note, int velocity);

    void update (void);

    // void note_on (uint8_t note);
    // void note_off (uint8_t note);
    // void notes_clear (void);
    // uint8_t get_notes_on (void);
}

