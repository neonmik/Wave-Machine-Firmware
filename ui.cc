#include "ui.h"

#include "drivers/adc.h"
#include "drivers/keys.h"
#include "drivers/button.h"
#include "drivers/leds.h"

#include "usb_descriptors.h"

namespace UI {

  void init (void) {

    LEDS::init();
    KEYS::init();
    ADC::init();

    CONTROLS::init();

    if (Buttons::PRESET.get(Buttons::State::SHIFT)) {
      // this currently brings up the Factory test/calibration mode, but should eventually bring up the MIDI/settings mode
      mode = UI_MODE_CALIBRATION;
      update(); // Call update() here so you can go through the routine and jump back into the startup process afterwards.
    }
    
    MIDI::init();

    printStartUp();
    
    poll = 0;
  }

  void update (void) { 
    switch (mode) {
      case UI_MODE_NORMAL:
        switch(poll) {
          case 0:
            NOTE_HANDLING::update();
            break;
          case 1:
            KEYS::read();
            break;
          case 2:
            KEYS::update();
            break;
          case 3:
            // this needs refactoring...

            // combonations go first so they don't muck up the single presses
            if (Buttons::PRESET.get(Buttons::State::SHIFT) && Buttons::PAGE.get(Buttons::State::SHORT)) {
              CONTROLS::save();
            }

            
            if (Buttons::ARP.get(Buttons::State::LONG)) {
              LEDS::ARP.flash(4,50);
              ARP::toggleHold();
            }
            if (Buttons::ARP.get(Buttons::State::SHORT)) {
              CONTROLS::toggleArp();
            }

            if (Buttons::LFO.get(Buttons::State::LONG)) {
              CONTROLS::toggleOSC();
            }
            if (Buttons::LFO.get(Buttons::State::SHORT)) {
              CONTROLS::toggleLFO();
            }

            if(Buttons::PAGE.get(Buttons::State::SHORT)) {
              CONTROLS::changePage();
            }

            if(Buttons::PRESET.get(Buttons::State::SHORT)) {
              CONTROLS::changePreset();
            }

            CONTROLS::setShift(Buttons::PAGE.get(Buttons::State::SHIFT));
            break;
          case 4:
            ADC::update();
            RANDOM::update(ADC::noise());
            break;
          case 5:
            PAGINATION::update();
            break;
          case 6:
            CONTROLS::update();
            break;
          case 7:
            LEDS::update();
            break;
          case 8:
            MIDI::update();
            break;
          default:
            // do nothing
            break;
        }

        ++poll;
        if (poll > 8) poll = 0;

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

  void printStartUp (void) {
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

    LEDS::test(50);

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
    ADC::update();
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
        ADC::update();
      }
      LEDS::KNOB_select(i, 1);
      LEDS::update();

      printf("Now turn Knob %d to 100%\n", i);
      
      while (ADC::value(i) != 1023) {
        sleep_ms(1);
        ADC::update();
      }
      LEDS::KNOB_select(i, 0);
      LEDS::update();
      printf("Knob %d check is complete!\n", i);
    }
    printf("All Knobs working correctly!\n\n");

    LEDS::test(50);

    mode = UI_MODE_NORMAL;
  }
}



    