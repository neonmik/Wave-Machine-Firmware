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
    ++_preset;
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



  void Init (void) {

    MIDI::Init();
    LEDS::Init();
    KEYS::Init();
    ADC::Init();
    CONTROLS::Init();
    PAGINATION::Init();

    if (Buttons::PRESET.get(Buttons::State::SHIFT)) {
      _mode = UI_MODE_CALIBRATION;
    }
    LEDS::LFO.set(get_lfo());
    LEDS::ARP.set(get_arp());
    
    print_startup();
    
    poll_index = 0;
  }

  void Update (void) { 
    switch (_mode) {
      case UI_MODE_NORMAL:
        switch(poll_index) {
          case 0:
            NOTE_HANDLING::Update();
            break;
          case 1:
            KEYS::read();
            break;
          case 2:
            KEYS::Update();
            break;
          case 3:
            // this needs tidying.

            // combonations go first so they don't muck up the single presses
            if (Buttons::PRESET.get(Buttons::State::SHIFT) && Buttons::PAGE.get(Buttons::State::SHORT)) {
                LEDS::PRESET.flash(4,50);
                CONTROLS::save();
                // printf("Save!\n");
            }
            if (Buttons::PRESET.get(Buttons::State::SHIFT) && Buttons::ARP.get(Buttons::State::SHORT)) {
                LEDS::ARP.flash(4,50);

                // printf("Arp!\n");
            }
            if (Buttons::PRESET.get(Buttons::State::SHIFT) && Buttons::LFO.get(Buttons::State::SHORT)) {
                LEDS::PAGE_1.flash_set(4,50);

                // printf("LFO!\n");
            }

            
            set_shift(Buttons::PAGE.get(Buttons::State::SHIFT));
            
            if (Buttons::ARP.get(Buttons::State::SHORT)) {
                toggle_arp();
            }

            if (Buttons::LFO.get(Buttons::State::SHORT)) {
                toggle_lfo();
            }
            break;
          case 4:
            ADC::Update();
            break;
          case 5:
            PAGINATION::Update();
            break;
          case 6:
            LEDS::Update();
            break;
          case 7:
            CONTROLS::Update();
            break;
          case 8:
            MIDI::Update();
            break;
          default:
            // do nothing
            break;
        }

        ++poll_index;
        if (poll_index > 8) poll_index = 0;

        break;

      case UI_MODE_FACTORY_TEST:
        break;

      case UI_MODE_CALIBRATION:
        calibrate();
        break;

      default:
        // do nothing
        break;
    }
  }

  void print_startup (void) {
    pico_unique_board_id_t board_id;
    pico_get_unique_board_id(&board_id);

    printf("\n\n");
    printf(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
    printf("~ ~ ~ ~ ~ ~ ~ ~ ~ Welcome to the Wave.... ~ ~ ~ ~ ~ ~ ~ ~\n");
    printf(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
    printf("~							~\n");
    printf("~	ID -		%02x %02x %02x %02x %02x %02x %02x %02x         ~\n", board_id.id[0], board_id.id[1], board_id.id[2], board_id.id[3], board_id.id[4], board_id.id[5], board_id.id[6], board_id.id[7]);
    printf("~	Firmware -	v%01.02f	      			~\n", VERSION);
    printf("~	Temp -		%02.01fºC				~\n", ADC::temp());
    printf("~							~\n");
    printf(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
    printf("~    		  © 2020-2023 NAMS Labs			~\n");
    printf(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
    printf("\n\n");
  }

  void calibrate (void) {

    LEDS::test(30);

    sleep_ms(1000);
    printf("\n\n");
    printf(" _______________________________________________________________________________\n");
    printf("| ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ |\n");
    printf("|-------------------------------------------------------------------------------|\n");
    printf("|										|\n");
    printf("|				Wave Machine Prototype				|\n");
    printf("|				      2020-2023					|\n");
    printf("|										|\n");
    printf("|		    Made by Nick Allott Musical Services (NAMS) Labs		|\n");
    printf("|										|\n");
    printf("|-------------------------------------------------------------------------------|\n");
    printf("| ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ |\n");
    printf("|_______________________________________________________________________________|\n\n");
    printf("Welcome to Factory Debug!\n\n");
    sleep_ms(300);
    printf("We will now test the hardware");
    ADC::Update();
    sleep_ms(300);

    printf(".");
    sleep_ms(300);
    printf(".");
    sleep_ms(300);
    printf(".");
    sleep_ms(300);
    printf("\n\n");

    LEDS::KNOBS_off();

    for (int i = 0; i < 4; i++) {
      printf("Please turn Knob %d to 0%\n", i);
      while (ADC::value(i) != 0) {
        sleep_ms(1);
        ADC::Update();
      }
      LEDS::KNOB_select(i, 1);
      LEDS::Update();

      printf("Now turn Knob %d to 100%\n", i);
      
      while (ADC::value(i) != 1023) {
        sleep_ms(1);
        ADC::Update();
      }
      LEDS::KNOB_select(i, 0);
      LEDS::Update();
      printf("Knob %d check is complete!\n", i);
    }
    printf("All Knobs working correctly!\n\n");

    LEDS::test(30);

    _mode = UI_MODE_NORMAL;
  }
}



    