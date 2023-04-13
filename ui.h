#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

#include "synth/settings.h"

#include "pagination.h"

#define KEYS_PRINT_OUT      0
#define KNOBS_PRINT_OUT     0
#define HARDWARE_TEST       0

namespace UI {

    enum UiMode {
        UI_MODE_NORMAL,
        UI_MODE_CALIBRATION,
        UI_MODE_FACTORY_TEST
    };

    namespace {
        // preset, page and event flags
        uint8_t _preset              =           0;
        uint8_t _page                =           0;
        
        bool page_flag              =           0;
        bool lfo_flag               =           0;
        bool arp_flag               =           0;
        bool preset_flag            =           0;
        bool shift_flag             =           0;

        uint8_t poll_index;

        UiMode _mode;
    }
    
    void set_page (uint8_t page);
    void set_page_flag(uint8_t value);
    uint8_t get_page_flag(void);

    void toggle_lfo(void);
    uint8_t get_lfo(void);

    void toggle_arp(void);
    uint8_t get_arp(void);

    void set_preset(uint8_t preset);
    void change_preset(void);
    uint8_t get_preset(void);

    void toggle_test_lfo (void);
    void toggle_test_arp (void);

    // ----------------------
    //        HARDWARE
    // ----------------------

    void init (void);
    void test (int delay);
    void update (void);
    void hardware_debug (void);
}