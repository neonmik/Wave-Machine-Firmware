#include "ui.h"


#include "drivers/adc.h"
#include "drivers/keys.h"
#include "drivers/button.h"
#include "drivers/leds.h"

#include "synth/arp.h"

extern uint8_t hardware_index;



#define MAX_PRESETS     7

namespace UI {

  // ----------------------
  //     PAGINATION/UI
  // ----------------------

  void pagination_init() {

    for(int i=0; i < MAX_KNOBS; i++){
      knob_values[i] = ADC::value(i);
      knob_states[i] = ACTIVE;
    }
  }
  // void default_pagination () {
  //   // ADSR default values
  //   page_values[Page::ADSR][0]=synth::attack_ms; //A
  //   page_values[Page::ADSR][1]=synth::decay_ms; // D
  //   page_values[Page::ADSR][2]=synth::sustain; // S
  //   page_values[Page::ADSR][3]=synth::release_ms; // R
  //   page_values[Page::LFO][0]=0; //A
  //   page_values[Page::LFO][1]=0; // D
  //   page_values[Page::LFO][2]=0; // S
  //   page_values[Page::LFO][3]=0; // R
  // }
  // read knobs and digital switches and handle pagination
  void pagination_protect() {
    LEDS::KNOBS.off();
    for(int i=0; i < MAX_KNOBS; i++){ // loop through the array and set all the values to protected.
      knob_states[i] = PROTECTED;
    }
  }
  void pagination_update(){
    if(Buttons::PAGE.get_short()){
      uint8_t pages = MAX_PAGES;

      page_change = true;

      current_page++;

      // LFO on
      // if (!get_lfo_flag()) pages--;
      // ARP on
      if (!get_arp()) pages--;
      
      // count the pages
      if (current_page >= pages) current_page = 0;
      
      set_page(current_page);
      pagination_protect();
    }
    if (Buttons::PRESET.get_short()) {
      change_preset();
      pagination_protect();
    }

    // read knobs values, show sync with the LED, enable knob when it matches the stored value
    for (int i = 0; i < MAX_KNOBS; i++){
      value = ADC::value(i);
      in_sync = abs(value - SETTINGS::get_value(current_page, i)) < protection_value;

      // enable knob when it matches the stored value
      if (in_sync){
        knob_states[i] = ACTIVE;
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
        // Leds::SR.set_pin(i);
        LEDS::KNOB_select(i, 1);
        SETTINGS::set_value(current_page, i, value);
        // page_values[current_page][i] = value;
        _touched = true;
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
    LEDS::RGB.preset(preset);

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

    LEDS::init();
    KEYS::init();
    ADC::init();
    SETTINGS::init();
    pagination_init();
    pagination_update();

    puts("Welcome to the jungle...");

    if (HARDWARE_TEST) test(10);

    hardware_index = 0;
  }

  void test (int delay) {
    
    LEDS::test(delay);
  }

  void update (void) {
    if (hardware_index == 0) KEYS::read();
    if (hardware_index == 1) {
      KEYS::update();
      if (Buttons::ARP.get_short()) toggle_arp();
      if (Buttons::LFO.get_short()) toggle_lfo();
    }
    if (hardware_index == 3) if (Arp::get()) Arp::update_playback();
    if (hardware_index == 3) ADC::update();
    if (hardware_index == 4) pagination_update();
    if (hardware_index == 5) LEDS::update();
    if (hardware_index == 6) SETTINGS::update();


    if (KNOBS_PRINT_OUT) {
      if (hardware_index==0) {
        print_knob_page();
        }
    }
    hardware_index++;
      //could be either of these, but apprently you can't loop 4/5/6 times like this...?
      // hardware_index &= 0x7;
    if (hardware_index > 6) hardware_index = 0; // this takes 2-3 more instructions to accomplish
  }
  void hardware_debug (void) {
    LEDS::SPARE.toggle();
  }
}