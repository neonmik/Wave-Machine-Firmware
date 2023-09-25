#pragma once

#include "pico/stdlib.h"
#include "pico/unique_id.h"

#include "midi.h"

#include "synth/controls.h"

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
        bool    _shift;
        
        bool page_flag              =           0;
        bool lfo_flag               =           0;
        bool arp_flag               =           0;
        bool preset_flag            =           0;
        bool shift_flag             =           0;

        uint8_t poll;
        uint8_t poll_index_last;

        uint32_t time_start;
        uint32_t time_end;
        uint32_t time_taken;

        UiMode _mode;
    }
    
    void setPage (uint8_t page);

    void toggleLFO(void);
    bool getLFO(void);

    void toggleArp(void);
    bool getArp(void);

    void change_preset(void);
    uint8_t get_preset(void);

    void printStartUp (void);

    // ----------------------
    //        HARDWARE
    // ----------------------

    void init (void);
    void update (void);
    void debug (void);
    void calibrate (void);
}