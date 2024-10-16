#include "ui.h"

#include "drivers/mux.h"
#include "drivers/adc.h"
#include "drivers/keys.h"
#include "drivers/button.h"
#include "drivers/leds.h"

#include "usb_descriptors.h"

#include "interval.h" 

bool startUpComplete;
bool calibrationCheck = false;

namespace UI {

  void init (void) {

    LEDS::init();
    MUX::init();
    KEYS::init();
    ADC::init();

    hardwareStartUp();

    if          (Buttons::PAGE.get(Buttons::State::SHIFT)) {
        if      (Buttons::FUNC1.get(Buttons::State::SHIFT))   { mode = UI_MODE_FACTORY_TEST;      update(); } // Call update() here so you can go through the routine and jump back into the startup process afterwards.
        else if (Buttons::FUNC2.get(Buttons::State::SHIFT))   { mode = UI_MODE_USB;               update(); }
        else if (Buttons::PRESET.get(Buttons::State::SHIFT))  { mode = UI_MODE_EXPORT_PRESETS;    update(); }
        else                                                  { mode = UI_MODE_CALIBRATION;       update(); }
    }

    startUpComplete = true;

    // TODO: #12 Fix false startup SYSEX Messages.
    MIDI::init();
    
    CONTROLS::init();
    CONTROLS::setupButtonAssignment();

    printStartUp();
  }

  void update (void) {
    switch (mode) {
      case UI_MODE_NORMAL:

        RANDOM::update(ADC::noise());
        NOTE_HANDLING::update();

        if (INTERVAL::UI.checkInterval()) {

          KEYS::update();
          PAGINATION::update();
          CONTROLS::update();
          LEDS::update();

        } else {
          KEYS::read();
          ADC::update();
          MUX::incrementAddress();
        }
        
        MIDI::update();

        break;

      case UI_MODE_FACTORY_TEST:
        while(mode == UI_MODE_FACTORY_TEST) {
          LEDS::test(40);
          LEDS::test(30);
          LEDS::test(20);
          LEDS::test(10);
          LEDS::test(20);
          LEDS::test(30);
          LEDS::test(40);
          
          mode = UI_MODE_NORMAL;
        }
        break;

      case UI_MODE_CALIBRATION:
        calibrate();
        break;

      case UI_MODE_USB:
        setUSBMode(true);

        printf("USB MSC Activated\n");
        while (getUSBMode()) {
          USB::update();
          // add a conditional here that if USB is ejected, it jumps back to normal operations
        }
        mode = UI_MODE_NORMAL;
        break;
      
      case UI_MODE_EXPORT_PRESETS:
        CONTROLS::exportAllPresets();
        mode = UI_MODE_NORMAL;
        break;


      default:
        // do nothing
        break;
    }
  
  }

void printStartUp (void) {

  pico_unique_board_id_t board_id;
  pico_get_unique_board_id(&board_id);

  for (int i = 0; i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES; i++) {
      hardwareID[i] = board_id.id[i];
  }

  printf("\n\n");
  printf(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
  printf("~ ~ ~ ~ ~ ~ ~ ~ ~ Welcome to the Wave.... ~ ~ ~ ~ ~ ~ ~ ~\n");
  printf(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
  printf("~							~\n");
  printf("~	ID -		%02x %02x %02x %02x %02x %02x %02x %02x         ~\n", hardwareID[0], hardwareID[1], hardwareID[2], hardwareID[3], hardwareID[4], hardwareID[5], hardwareID[6], hardwareID[7]);
  printf("~	Firmware -	v%01.02f	      			~\n", VERSION);
  printf("~	Temp -		%02.01fºC				~\n", ADC::temperature());
  printf("~	Battery -	%.02fV				~\n", ADC::battery());
  printf("~							~\n");
  printf(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
  printf("~    		  © 2020-2024 NAMS Labs			~\n");
  printf(" ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~\n");
  printf("\n\n");
}

void calibrate (void) {

  LEDS::test(50);

  MUX::setAddress(0);

  sleep_ms(1000);
  
  calibrationCheck = true;


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

    volatile uint16_t knobValue = ADC::value(i);

    while (knobValue == 0) {
      sleep_ms(10);
      ADC::update();
      knobValue = ADC::value(i);
    }

    LEDS::KNOB_select(i, 1);
    LEDS::update();

    printf("Now turn Knob %d to 100%\n", i);
    
    knobValue = ADC::value(i);

    while (knobValue == 1023) {
      sleep_ms(10);
      ADC::update();
      knobValue = ADC::value(i);
    }

    MUX::incrementAddress();
    
    LEDS::KNOB_select(i, 0);
    LEDS::update();

    printf("Knob %d check is complete!\n", i);
  }
  printf("All Knobs working correctly!\n\n");

  LEDS::test(50);

  calibrationCheck = false; 

  mode = UI_MODE_NORMAL;
  return;
}

void hardwareStartUp (void) {
    // Have to do this to prime all the hardware
    for (int i = 0; i < 8; i++) {
      for (int i = 0; i < 16; i++) {
        KEYS::read();
        ADC::update();
        MUX::incrementAddress();
        sleep_ms(5);
      }
    }
    KEYS::update();
  }
}



    