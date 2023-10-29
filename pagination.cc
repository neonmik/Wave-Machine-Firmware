#include "pagination.h"

namespace PAGINATION {
  
    void init() {
      update();
    }
    void refresh() {
        LEDS::KNOBS.off();
        for(int i = 0; i < MAX_KNOBS; i++) { // loop through the array and set all the values to protected.
            protect(i);
            clear(i);
        }
    }

    // void setPage (uint8_t page) {
    //   currentPage = page;
    //   refresh();
    // }

    void update(){
      uint8_t activePage = CONTROLS::getPage();

      // if (CONTROLS::getShift()) {
      //   activePage += MAX_PAGES;
      //   refresh();
      // } 
      
      // read knobs values, show sync with the LED, enable knob when it matches the stored value
      for (int i = 0; i < MAX_KNOBS; i++){
        currentValue = ADC::value(i);

        in_sync = abs(currentValue - CONTROLS::getKnob(activePage, i)) < knob_protection_threshold;

        // enable knob when it matches the stored value
        if (in_sync && currentValue != lastValue[i]) {
            currentState[i] = KnobState::ACTIVE;
            lastValue[i] = currentValue;
        }
      
        // if enabled then mirror the real time knob value
        if(currentState[i] == KnobState::ACTIVE){
          LEDS::KNOB_select(i, 1);
          // this could be where you set the level for KNOB LED's faux PWM output
          CONTROLS::setKnob(activePage, i, currentValue);
        }
      }
    }
  }
  
