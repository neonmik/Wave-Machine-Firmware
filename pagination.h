#pragma once

#include "pico/stdlib.h"

#include "ui.h"

#include "drivers/button.h"
#include "drivers/leds.h"
#include "drivers/adc.h"


#define MAX_PAGES         4 // the max number of pages available
#define MAX_KNOBS         4 // the max number of knobs available

#define protection_value  10 // the amount of protection the knob gets before unlocking.



namespace PAGINATION {

    enum class KnobState {
        PROTECTED,
        ACTIVE
    };
    
    namespace {
        bool page_change = false;
        uint8_t current_page = 0;
        
        KnobState _states[MAX_KNOBS];
        uint32_t last_value[MAX_KNOBS];
        uint32_t value;
        bool in_sync;
    }
    void init();
    void protect();
    void update();
}
  
