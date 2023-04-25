/**
 *            Beep Machine Firmware v0.25
 * 
 * Copyright (c) 2022 Nick Allott Musical Services (NAMS)
 *  
 */


#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "synth/modulation.h"
#include "synth/arp.h"
#include "synth/note_priority.h"
#include "synth/synth.h"

#include "drivers/dac.h"

#include "ui.h"

#include "mailbox.h"

#define SAMPLE_RATE     48000
#define MAX_VOICES      8
#define BPM             120 // part of a function for determinding bpm ms

uint32_t sample_clock = 0;
uint32_t sample_clock_last = 0;

uint32_t software_index = 0;




void core1_main() {

  UI::init();
  sleep_ms(100);

  while (true) {

    UI::update();

  }
}


void core0_main() {


  SYNTH::init(SAMPLE_RATE);
  MOD::init();
  DAC::init(SAMPLE_RATE, SYNTH::get_audio_frame);
  ARP::init(BPM, SAMPLE_RATE);
  

  uint8_t index = 0;
  while (true) {
    
    if (DAC::get_state()) {
    
      // MOD::update();
      MAILBOX::receive(); //copy the data from the mailbox to the local variables
      NOTE_PRIORITY::update(); // update notes from the mailbox info
      
      DAC::clear_state();
    }
  }
}

int main() {

  set_sys_clock_khz(144000, true); 

  stdio_init_all(); // has to be here to allow both cores to use the UART

  MAILBOX::init();

  multicore_launch_core1(core1_main);

  core0_main();
  
} 


