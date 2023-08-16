#pragma once

#include "pico/stdlib.h"

#include "../queue.h"
#include "../config.h"

#include "arp.h"






namespace NOTE_HANDLING {

    enum class Priority {
        FIRST,      // First/oldest notes stay the longest
        LAST,       // Last/newest notes stay the longest
        HIGHEST,    // Highest notes stay the longest
        LOWEST      // Lowest notes stay the longest
    };

    namespace {

        // Q16 representations the frequency of all MIDI notes
        const uint32_t note2freq[128] = {
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
        
        bool        _sustain;
        // uint8_t     _held_notes[POLYPHONY];
        // volatile uint8_t     _num_held_notes;

        int8_t     _voices_active = 0;


        struct voice_data_t {
            uint8_t     note; // holds MIDI note number
            bool        gate; // tracks whether note is being physcially played
            bool        active; // tracks whether the note is still sounding out
            uint32_t    activation_time; // time when the note was activated

            void on (uint8_t _note) {
                note = _note;
                gate = true;
                active = true;
                activation_time = to_ms_since_boot(get_absolute_time());
            }
            void off (void) {
                gate = false;
            }
            void clear (void) {
                note = 0;
                active = false;
                activation_time = 0;
            }
        };
    }

    void voices_inc (void);
    void voices_dec (void);
    bool voices_active (void);
    
    extern voice_data_t VOICES[POLYPHONY];
    // actual synth voice notes, also add MIDI out here
    void voice_on(int slot, int note, int velocity);
    void voice_off(int slot, int note, int velocity);
    void voices_panic(void);

    // filter env
    void filter_on (void);
    void filter_off (void);

    // Note priority detection
    void priority(int status, int note, int velocity);
    void release(int note, int velocity);

    void update (void);

    void check_release (void);

    void note_on (uint8_t note, uint8_t velocity);
    void note_off (uint8_t note, uint8_t velocity);
    void notes_clear (void);
    uint8_t get_notes_on(void);

    void sustain_pedal (uint16_t status);
}

