#pragma once

#include "config.h"

#include "pico/unique_id.h"

#include "midi.h"

#include "controls.h"

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
        uint8_t     poll;

        UiMode      mode;
    }

    void printStartUp (void);

    // ----------------------
    //        HARDWARE
    // ----------------------

    void init (void);
    void update (void);
    void debug (void);
    void calibrate (void);
}