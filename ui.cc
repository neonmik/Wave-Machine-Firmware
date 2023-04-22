#include "ui.h"


#include "drivers/adc.h"
#include "drivers/keys.h"
#include "drivers/button.h"
#include "drivers/leds.h"

// #include "synth/arp.h"


namespace UI {

  void set_page (uint8_t value) {
    // using a switch here so that I can easily change the LEDs... find a better way?
    switch (value) {
      case 0:
        _page = 0;
        LEDS::PAGES.off();
        SETTINGS::set_page(_page);
        break;
      case 1:
        _page = 1;
        LEDS::PAGE_1.toggle();
        SETTINGS::set_page(_page);
        break;
      case 2:
        _page = 2;
        LEDS::PAGE_1.toggle();
        LEDS::PAGE_2.toggle();
        SETTINGS::set_page(_page);
        break;
      case 3:
        _page = 3;
        LEDS::PAGE_2.toggle();
        LEDS::PAGE_3.toggle();
        SETTINGS::set_page(_page);
        break;
    }
  }
  uint8_t get_page(void) {
    return _page;
  }
 
  void toggle_lfo(void) {
    LEDS::LFO.toggle();
    SETTINGS::toggle_lfo();
  }
  uint8_t get_lfo(void) {
    return SETTINGS::get_lfo();
  }

  void toggle_test_lfo (void) {
    LEDS::LFO.toggle();
  }
  void toggle_test_arp (void) {
    LEDS::ARP.toggle();
  }

  
  void toggle_arp(void) {
    LEDS::ARP.toggle();
    SETTINGS::toggle_arp();
  }
  uint8_t get_arp(void) {
    return SETTINGS::get_arp();
  }

  void change_preset(void) {
    _preset++;
    _preset&=0x7;
    SETTINGS::set_preset(_preset);
    LEDS::PRESET.preset(_preset);

    LEDS::ARP.off();
    LEDS::LFO.off();
    if (SETTINGS::get_arp()) LEDS::ARP.on();
    if (SETTINGS::get_lfo()) LEDS::LFO.on();
  }
  uint8_t get_preset(void) {
    return _preset;
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

    if (Buttons::PRESET.get(Buttons::State::SHIFT)) test(30);

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
            if (Buttons::ARP.get(Buttons::State::SHORT)) {
                toggle_arp();
            }
            if (Buttons::LFO.get(Buttons::State::SHORT)) {
                toggle_lfo();
            }
            if (Buttons::PRESET.get(Buttons::State::SHIFT) && Buttons::PAGE.get(Buttons::State::SHORT)) {
                LEDS::PRESET.flash(4,50);
                SETTINGS::save();
            }
            // if (Buttons::ARP.get(Buttons::ButtonState::LONG)) {
            //     SETTINGS::toggle_hold();
            //     LEDS::ARP.flash(2,50);
            // }
            break;
          case 2:
            ADC::update();
            PAGINATION::update();
            break;
          case 3:
            LEDS::update();
            break;
          case 4:
            SETTINGS::update();
            break;
          default:
            // do nothing
            break;
        }

        ++poll_index;
        if (poll_index > 4) poll_index = 0;
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


    