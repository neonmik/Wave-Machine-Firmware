#pragma once

#include "config.h"

#include "pico/unique_id.h"

#include "midi.h"

#include "controls.h"

#include "pagination.h"

#define KEYS_PRINT_OUT      0
#define KNOBS_PRINT_OUT     0
#define HARDWARE_TEST       0

#define SHIFT_TIMEOUT       512

namespace UI {

    enum UiMode {
        UI_MODE_NORMAL,
        UI_MODE_CALIBRATION,
        UI_MODE_FACTORY_TEST
    };

    namespace {
        uint8_t currentPreset              =           0;
        uint8_t currentPage                =           0;
        bool    shift;
        uint16_t shiftCounter;

        uint8_t poll;

        UiMode _mode;
    }
    
    void setPage (uint8_t page);

    void toggleLFO(void);
    bool getLFO(void);

    void toggleArp(void);
    bool getArp(void);

    void changePreset(void);
    uint8_t getPreset(void);

    void printStartUp (void);

    // ----------------------
    //        HARDWARE
    // ----------------------

    void init (void);
    void update (void);
    void debug (void);
    void calibrate (void);
}