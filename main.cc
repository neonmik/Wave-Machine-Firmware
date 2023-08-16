/**
 *                Wave Machine Firmware v0.29
 * 
 * Copyright (c) 2022-2023 Nick Allott Musical Services (NAMS)
 *  
 */


#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "config.h"

#include "synth/arp.h"
#include "synth/synth.h"

#include "drivers/dac.h"

#include "ui.h"

#include "queue.h"

uint32_t sample_clock;

void hw_core() {
  
  UI::init();

  while (true) {

    UI::update();

  }
}


void synth_core() {

  SYNTH::init();
  DAC::init(SYNTH::get_audio_frame);
  // CLOCK::init();
  
  while (true) {
     if (DAC::get_state()) {
      
      // tidy this...
      uint8_t temp = QUEUE::trigger_check_queue();
      if (temp) {
        for (int i = 0; i < temp; i++){
          uint8_t slot_ = 0;
          uint8_t note_ = 0;
          bool gate_ = 0;

          QUEUE::trigger_receive(slot_, note_, gate_);
            if (slot_ < POLYPHONY) {
              if (gate_) SYNTH::voice_on(slot_, note_);
              if (!gate_) SYNTH::voice_off(slot_);
            } else {
              if (gate_) FILTER::trigger_attack();
              if (!gate_) FILTER::trigger_release();
            }
        }
      }
      
      DAC::clear_state();
    }
  }
}

 int main() {

  set_sys_clock_khz(CORE_SPEED, true); // needs to be called before UART. Extra speed is needed to run the code.

  stdio_init_all(); // has to be here to allow both cores to use the debug serial UART. 

  QUEUE::init(); // has to be here to allow both cores access to QUEUE
  
  multicore_launch_core1(hw_core); // launches the hardware core

  synth_core(); // launches the synth/audio core

} 

