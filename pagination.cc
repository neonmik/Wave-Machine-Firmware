#include "pagination.h"

namespace PAGINATION {
  
    void init() {
      update();
    }
    void refresh() {
        // LEDS::KNOBS.off();
        for(int i = 0; i < MAX_KNOBS; i++) { // loop through the array and set all the values to protected.
            LEDS::KNOB_select(i, 0);
            protect(i);
            clear(i);
        }
    }

    void update(){
      uint8_t activePage = CONTROLS::getPage();

      // read knobs values, show sync with the LED, enable knob when it matches the stored value
      for (int i = 0; i < MAX_KNOBS; i++){
        currentValue = ADC::value(i);

        in_sync = abs(currentValue - CONTROLS::getKnob(activePage, i)) < PROTECTION_THRESHOLD;

        // enable knob when it matches the stored value
        if (in_sync && currentValue != lastValue[i]) {
            if (currentState[i] != KnobState::ACTIVE) {
              // only light up if not already lit
              currentState[i] = KnobState::ACTIVE;
              LEDS::KNOB_select(i, 1);
            }

            lastValue[i] = currentValue;
            
            // this could be where you set the level for KNOB LED's PWM output eventually
        }
      
        // if enabled then mirror the real time knob value
        if(currentState[i] == KnobState::ACTIVE){
          CONTROLS::setKnob(activePage, i, currentValue);
        }
      }
    }
  }
  
