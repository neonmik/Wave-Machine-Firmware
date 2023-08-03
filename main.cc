/**
 *                Beep Machine Firmware v0.28
 * 
 * Copyright (c) 2022-2023 Nick Allott Musical Services (NAMS)
 *  
 */


#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "config.h"

#include "synth/arp.h"
// #include "synth/note_priority.h"
#include "synth/synth.h"

#include "drivers/dac.h"

#include "ui.h"

#include "queue.h"


void hw_core() {
  
  UI::init();

  while (true) {

    UI::update();

  }
}


void synth_core() {

  SYNTH::init();
  DAC::init(SYNTH::get_audio_frame);
  ARP::init();
  
  while (true) {
     if (DAC::get_state()) {
      
      uint8_t temp = QUEUE::trigger_check_queue();
      if (temp) {
        for (int i = 0; i < temp; i++){
          uint8_t slot_ = 0;
          uint8_t note_ = 0;
          bool gate_ = false;

          QUEUE::trigger_receive(slot_, note_, gate_);
            if (slot_ < MAX_VOICES) {
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

  stdio_init_all(); // has to be here to allow both cores to use the UART

  QUEUE::init(); // has to be here to allow both cores access to MAILBOX
  
  multicore_launch_core1(hw_core); 

  synth_core();

} 

