#include "ui.h"

#include "drivers/adc.h"
#include "drivers/keys.h"
#include "drivers/button.h"
#include "drivers/leds.h"

namespace UI {

  void set_page (uint8_t value) {
    // using a switch here so that I can easily change the LEDs... find a better way?
    _page = value;
    switch (_page) {
      case 0:
        LEDS::PAGES.off();
        CONTROLS::set_page(_page);
        break;
      case 1:
        LEDS::PAGE_1.toggle();
        CONTROLS::set_page(_page);
        break;
      case 2:
        LEDS::PAGE_1.toggle();
        LEDS::PAGE_2.toggle();
        CONTROLS::set_page(_page);
        break;
      case 3:
        LEDS::PAGE_2.toggle();
        LEDS::PAGE_3.toggle();
        CONTROLS::set_page(_page);
        break;
    }
  }
  uint8_t get_page(void) {
    return _page;
  }
 
  void toggle_lfo(void) {
    CONTROLS::toggle_lfo();
    LEDS::LFO.set(CONTROLS::get_lfo());
  }
  bool get_lfo(void) {
    return CONTROLS::get_lfo();
  }

  void toggle_test_lfo (void) {
    LEDS::LFO.toggle();
  }
  void toggle_test_arp (void) {
    LEDS::ARP.toggle();
  }

  
  void toggle_arp(void) {
    CONTROLS::toggle_arp();
    LEDS::ARP.set(CONTROLS::get_arp());
  }
  bool get_arp(void) {
    return CONTROLS::get_arp();
  }

  void change_preset(void) {
    _preset++;
    if (_preset >= MAX_PRESETS) _preset = 0;
    CONTROLS::set_preset(_preset);
    LEDS::PRESET.preset(_preset);

    LEDS::ARP.off();
    LEDS::LFO.off();
    if (CONTROLS::get_arp()) LEDS::ARP.on();
    if (CONTROLS::get_lfo()) LEDS::LFO.on();
  }
  uint8_t get_preset(void) {
    return _preset;
  }

void set_shift (bool shift) {
  if (_shift != shift) {
    CONTROLS::toggle_shift();
    _shift = shift;
  }
}

  // ----------------------
  //        HARDWARE
  // ----------------------


  void init (void) {
    // stdio_init_all();

    printf("\nWelcome to the jungle...\n");
    // printf("\ncore1 here!\n");
    MIDI::init();

    LEDS::init();
    KEYS::init();
    ADC::init();
    CONTROLS::init();
    PAGINATION::init();

    if (Buttons::PRESET.get(Buttons::State::SHIFT)) test(30);

    LEDS::LFO.set(get_lfo());
    LEDS::ARP.set(get_arp());

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
            set_shift(Buttons::PAGE.get(Buttons::State::SHIFT));

            if (Buttons::PRESET.get(Buttons::State::SHIFT) && Buttons::PAGE.get(Buttons::State::SHORT)) {
                LEDS::PRESET.flash(4,50);
                CONTROLS::save();
            }
            // if (Buttons::ARP.get(Buttons::ButtonState::LONG)) {
            //     CONTROLS::toggle_hold();
            //     LEDS::ARP.flash(2,50);
            // }
            break;
          case 2:
            ADC::update();
          case 3:
            PAGINATION::update();
            break;
          case 4:
            LEDS::update();
            break;
          case 5:
            CONTROLS::update();
            break;
          case 6:
            MIDI::update();
            break;
          default:
            // do nothing
            break;
        }

        ++poll_index;
        if (poll_index > 6) poll_index = 0;
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


    