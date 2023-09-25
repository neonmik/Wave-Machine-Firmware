#include "pagination.h"

namespace PAGINATION {
  
    void init() {
      update();
    }
    void update(){
      uint8_t temp_page;

      if(Buttons::PAGE.get(Buttons::State::SHORT)){
        
        uint8_t pages = MAX_PAGES;
        if (!UI::getArp()) pages = (MAX_PAGES - 1);


        ++current_page;
        if (current_page >= pages) current_page = 0;

        UI::setPage(current_page);
        refresh();
      }

      if (Buttons::PRESET.get(Buttons::State::SHORT)) {
        UI::change_preset();
        refresh();
      }

      if (Buttons::PAGE.get(Buttons::State::SHIFT)) {
        temp_page = (current_page + 4);
        refresh();
      } else {
        temp_page = current_page;
        refresh();
      }
      
      // read knobs values, show sync with the LED, enable knob when it matches the stored value
      for (int i = 0; i < MAX_KNOBS; i++){
        value = ADC::value(i);

        in_sync = abs(value - CONTROLS::getValue(temp_page, i)) < knob_protection_threshold;

        // enable knob when it matches the stored value
        if (in_sync && value != last_value[i]) {
            _states[i] = KnobState::ACTIVE;
            last_value[i] = value;
        }
      
        // if enabled then mirror the real time knob value
        if(_states[i] == KnobState::ACTIVE){
          LEDS::KNOB_select(i, 1);
          // this could be where you set the level for KNOB LED's faux PWM output
          CONTROLS::setValue(temp_page, i, value);
        }
      }
    }
  }
  
