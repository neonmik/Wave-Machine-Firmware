/**
 *                Beep Machine Firmware v0.26
 * 
 * Copyright (c) 2022-2023 Nick Allott Musical Services (NAMS)
 *  
 */


#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "config.h"

#include "synth/arp.h"
#include "synth/note_priority.h"
#include "synth/synth.h"

#include "drivers/dac.h"

#include "ui.h"

#include "mailbox.h"




void core1_main() {

  UI::init();
  sleep_ms(100);

  while (true) {

    UI::update();

  }
}


void core0_main() {

  SYNTH::init(SAMPLE_RATE);
  DAC::init(SAMPLE_RATE, SYNTH::get_audio_frame);
  ARP::init(DEFAULT_BPM, SAMPLE_RATE);
  
  while (true) {
    if (DAC::get_state()) {
    
      MAILBOX::receive(); //copy the data from the mailbox to the local variables
      NOTE_PRIORITY::update(); // update notes from the mailbox info
      
      DAC::clear_state();
      
    }
  }
}

int main() {

  set_sys_clock_khz(CORE_SPEED, true); // needs to be called before UART. Not sure if the extra speed is needed to run the code, but it gives it a little headroom.
  stdio_init_all(); // has to be here to allow both cores to use the UART

  MAILBOX::init(); // has to be here to allow both cores access to MAILBOX

  multicore_launch_core1(core1_main); 

  core0_main();
  
} 


