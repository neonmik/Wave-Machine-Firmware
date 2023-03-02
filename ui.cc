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
  
  // ----------------------
  //       FLAGS/UI
  // ----------------------

  void set_page (uint8_t value) {
    // using a switch here so that I can easily change the LEDs... find a better way?
    switch (value) {
      case 0:
        page = 0;
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

    poll_index = 0;
  }

  void test (int delay) {
    
    LEDS::test(delay);
  }

  void update (void) {
    switch (_mode) {
      case UI_MODE_NORMAL:
        switch(poll_index) {
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

        ++poll_index;
        if (poll_index > 6) {
            poll_index = 0;
        }
        break;

      case UI_MODE_FACTORY_TEST:
        hardware_debug();
        break;

      case UI_MODE_CALIBRATION:
        break;

      default:
        // do nothing
        break;
    }
  }
}


    