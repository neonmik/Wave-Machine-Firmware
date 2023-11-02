#pragma once

#include "config.h"

#include "ui.h"

#include "drivers/button.h"
#include "drivers/leds.h"
#include "drivers/adc.h"


namespace PAGINATION {

    enum class KnobState {
        PROTECTED,
        ACTIVE
    };
    namespace {
        uint8_t currentPage = 0;
        
        KnobState currentState[MAX_KNOBS];
        uint16_t currentValue;
        int16_t lastValue[MAX_KNOBS];
        bool in_sync;

        
        void protect(int value) {
            currentState[value] = KnobState::PROTECTED;
        }
        void clear(int value) {
            lastValue[value] = -1; //-1 so if the page/preset has changed, it's never the same value
        }
    }
    void init(void);
    void update(void);
    void refresh(void);

    void setPage (uint8_t page);
}
  
