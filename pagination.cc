#include "pagination.h"

namespace PAGINATION {
    void init() {
      for(int i=0; i < MAX_KNOBS; i++){
        // knob_values[i] = ADC::value(i);
        _states[i] = KnobState::ACTIVE;
      }
      update();
    }
    void protect() {
      LEDS::KNOBS.off();
      for(int i=0; i < MAX_KNOBS; i++){ // loop through the array and set all the values to protected.
        _states[i] = KnobState::PROTECTED;
      }
    }
    void update(){
      if(Buttons::PAGE.get(Buttons::ButtonState::SHORT)){
        uint8_t pages = MAX_PAGES;

        page_change = true;

        ++current_page;

        // LFO on
        // doesnt work correctly, need to be selective about pages available, or use another method
        // if (!get_lfo_flag()) pages--;
        // ARP on
        if (!UI::get_arp()) pages--;
        
        // count the pages
        if (current_page >= pages) current_page = 0;
        
        UI::set_page(current_page);
        protect();
      }

      if (Buttons::PRESET.get(Buttons::ButtonState::SHORT)) {
        UI::change_preset();
        protect();
      }

      // read knobs values, show sync with the LED, enable knob when it matches the stored value
      for (int i = 0; i < MAX_KNOBS; i++){
        value = ADC::value(i);
        in_sync = abs(value - SETTINGS::get_value(current_page, i)) < protection_value;

        // enable knob when it matches the stored value
        if (in_sync && value != last_value[i]) {
            _states[i] = KnobState::ACTIVE;
            last_value[i] = value;
        }
      
        // if knob is moving, show if it's active or not
        //  if(abs(value - knob_values[i]) > 5){
        //       // if knob is active, blink LED
        //       if(knob_states[i] == ACTIVE){
        //         Leds::KNOB_select(i, 1);
        //         continue;
        //       } else {
        //         Leds::KNOB_select(i, 0);
        //       }
        //  }
        // knob_values[i] = value;

        // if enabled then mirror the real time knob value
        if(_states[i] == KnobState::ACTIVE){
          LEDS::KNOB_select(i, 1);
          SETTINGS::set_value(current_page, i, value);
        }
      }
    }
  }
  
