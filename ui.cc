#include "ui.h"


#include "drivers/adc.h"
#include "drivers/keys.h"
#include "drivers/button.h"
#include "drivers/leds.h"

#include "synth/arp.h"


namespace UI {

  // ----------------------
  //     PAGINATION/UI
  // ----------------------
  namespace PAGINATION {
    void init() {
      for(int i=0; i < MAX_KNOBS; i++){
        // knob_values[i] = ADC::value(i);
        knob_states[i] = ACTIVE;
      }
      update();
    }
    void protect() {
      LEDS::KNOBS.off();
      for(int i=0; i < MAX_KNOBS; i++){ // loop through the array and set all the values to protected.
        knob_states[i] = PROTECTED;
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
        if (!get_arp()) pages--;
        
        // count the pages
        if (current_page >= pages) current_page = 0;
        
        set_page(current_page);
        protect();
      }

      if (Buttons::PRESET.get(Buttons::ButtonState::SHORT)) {
        change_preset();
        protect();
      }

      // read knobs values, show sync with the LED, enable knob when it matches the stored value
      for (int i = 0; i < MAX_KNOBS; i++){
        value = ADC::value(i);
        in_sync = abs(value - SETTINGS::get_value(current_page, i)) < protection_value;

        // enable knob when it matches the stored value
        if (in_sync && value != last_value[i]) {
            knob_states[i] = ACTIVE;
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
        if(knob_states[i] == ACTIVE){
          LEDS::KNOB_select(i, 1);
          SETTINGS::set_value(current_page, i, value);
        }
      }
      
    }
  }
  

  // ----------------------
  //       FLAGS/UI
  // ----------------------

  void toggle_shift_flag (void) {
    shift_flag = !shift_flag;
  }
  bool get_shift_flag (void){
    return shift_flag;
  }

  void set_page (uint8_t value) {
    // using a switch here so that I can easily change the LEDs... find a better way?
    switch (value) {
      case 0:
        page = 0;
        // Leds::PAGES_off();
        LEDS::PAGES.off();
        SETTINGS::set_page(page);
        break;
      case 1:
        page = 1;
        LEDS::PAGE_1.toggle();
        SETTINGS::set_page(page);
        break;
      case 2:
        page = 2;
        LEDS::PAGE_1.toggle();
        LEDS::PAGE_2.toggle();
        SETTINGS::set_page(page);
        break;
      case 3:
        page = 3;
        LEDS::PAGE_2.toggle();
        LEDS::PAGE_3.toggle();
        SETTINGS::set_page(page);
        break;
    }
  }
  uint8_t get_page(void) {
    return page;
  }
 
  void toggle_lfo(void) {
    LEDS::LFO.toggle();
    SETTINGS::toggle_lfo();
    if (KEYS_PRINT_OUT) printf("Key: LFO\n");
  }
  uint8_t get_lfo(void) {
    return SETTINGS::get_lfo();
  }

  
  void toggle_arp(void) {
    LEDS::ARP.toggle();
    SETTINGS::toggle_arp();
  }
  uint8_t get_arp(void) {
    return SETTINGS::get_arp();
  }

  void change_preset(void) {
    preset++;
    preset&=0x7;
    SETTINGS::set_preset(preset);
    LEDS::PRESET.preset(preset);

    LEDS::ARP.off();
    LEDS::LFO.off();
    if (SETTINGS::get_arp()) LEDS::ARP.on();
    if (SETTINGS::get_lfo()) LEDS::LFO.on();
  }
  uint8_t get_preset(void) {
    return preset;
  }
  void set_preset_flag(uint8_t value) {
    preset_flag = value;
  }
  uint8_t get_preset_flag(void) {
      return preset_flag;
    }


  // ----------------------
  //        HARDWARE
  // ----------------------


  void init (void) {
    stdio_init_all();

    puts("Welcome to the jungle...");

    LEDS::init();
    KEYS::init();
    ADC::init();
    SETTINGS::init();
    PAGINATION::init();

    if (Buttons::PRESET.get(Buttons::ButtonState::SHIFT)) test(10);

    if (HARDWARE_TEST) test(10);

    hardware_index = 0;
  }

  void test (int delay) {
    
    LEDS::test(delay);
  }

void update (void) {
    switch(hardware_index) {
      case 0:
        KEYS::read();
        break;
      case 1:
        KEYS::update();
        if (Buttons::ARP.get(Buttons::ButtonState::SHORT)) {
            toggle_arp();
        }
        if (Buttons::LFO.get(Buttons::ButtonState::SHORT)) {
            toggle_lfo();
        }
        if (Buttons::PRESET.get(Buttons::ButtonState::SHIFT) && Buttons::PAGE.get(Buttons::ButtonState::SHORT)) {
            LEDS::PRESET.flash(4,50);
            printf("Save!!\n");
        }
        break;
      case 2:
        if (ARP::get()) {
            ARP::update_playback();
        }
      case 3:
        ADC::update();
        break;
      case 4:
        PAGINATION::update();
        break;
      case 5:
        LEDS::update();
        break;
      case 6:
        SETTINGS::update();
        break;
      default:
        // do nothing
        break;
    }

    ++hardware_index;
    if (hardware_index > 6) {
        hardware_index = 0;
    }
}

  void hardware_debug (void) {
    LEDS::SPARE.toggle();
  }
}



    