#pragma once

#include "pico/stdlib.h"

#include "ui.h"

#include "drivers/button.h"
#include "drivers/leds.h"
#include "drivers/adc.h"


#define abs(x) ({ __typeof__(x) _x = (x); _x >= 0 ? _x : -_x; })

#define MAX_PAGES         4 // the max number of pages available
#define MAX_KNOBS         4 // the max number of knobs available

namespace PAGINATION {

    enum class KnobState {
        PROTECTED,
        ACTIVE
    };
    
    namespace {
        // bool page_change = false;
        uint8_t current_page = 0;
        
        KnobState _states[MAX_KNOBS];
        int16_t last_value[MAX_KNOBS];
        uint16_t value;
        bool in_sync;

        uint16_t knob_protection_threshold = 10; // the amount of protection the knob gets before unlocking.
        
        void protect(int value) {
            _states[value] = KnobState::PROTECTED;
        }
        void clear(int value) {
            last_value[value] = -1; //-1 so if the page/preset has changed, it's never the same value
        }
        void refresh() {
            LEDS::KNOBS.off();
            for(int i=0; i < MAX_KNOBS; i++){ // loop through the array and set all the values to protected.
            protect(i);
            clear(i);
            }
        }

    }
    void init(void);
    void update(void);
}
  
