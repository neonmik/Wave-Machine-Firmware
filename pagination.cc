#include "pagination.h"

namespace PAGINATION {
    namespace {
      void refresh() {
        LEDS::KNOBS.off();
        for(int i=0; i < MAX_KNOBS; i++){ // loop through the array and set all the values to protected.
          protect(i);
          clear(i);
        }
      }
      void protect(int value) {
        _states[value] = KnobState::PROTECTED;
      }
      void clear(int value) {
        last_value[value] = -1; //-1 so if the page/preset has changed, it's never the same value
      }
    }
    void init() {
      update();
    }
    void update(){
      if(Buttons::PAGE.get(Buttons::State::SHORT)){
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
        refresh();
      }

      if (Buttons::PRESET.get(Buttons::State::SHORT)) {
        UI::change_preset();
        refresh();
      }

      // read knobs values, show sync with the LED, enable knob when it matches the stored value
      for (int i = 0; i < MAX_KNOBS; i++){
        value = ADC::value(i);
        in_sync = abs(value - SETTINGS::get_value(current_page, i)) < knob_protection_threshold;

        // enable knob when it matches the stored value
        if (in_sync && value != last_value[i]) {
            _states[i] = KnobState::ACTIVE;
            last_value[i] = value;
        }
      
        // if enabled then mirror the real time knob value
        if(_states[i] == KnobState::ACTIVE){
          LEDS::KNOB_select(i, 1);
          // this could be where you set the level for KNOB LED's faux PWM output
          SETTINGS::set_value(current_page, i, value);
        }
      }
    }
  }
  
