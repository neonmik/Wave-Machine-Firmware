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

#define SAMPLE_RATE     44100
#define MAX_VOICES      8
#define BPM             120 // part of a function for determinding bpm ms

uint32_t sample_clock = 0;
uint32_t sample_clock_last = 0;

uint32_t software_index = 0;

bool update_flag = false;


void core1_main() {

  UI::init();
  sleep_ms(100);

  while (true) {

    UI::update();

  }
}


void core0_main() {

  SYNTH::init(SAMPLE_RATE);
  MOD::init(SAMPLE_RATE);
  ARP::init(BPM, SAMPLE_RATE);
  DAC::init(SAMPLE_RATE, SYNTH::get_audio_frame);
  
  // sleep_ms(10);
  uint8_t index = 0;
  while (true) {
  
    if (DAC::get_state()) {
      MOD::update();
      switch (index) {
        case 0:
          MAILBOX::receive(); //copy the data from the mailbox to the local variables
          index++;
          break;
        case 1:
          NOTE_PRIORITY::update(); // update notes from the mailbox info
          index = 0;
          break;
      }
      
      DAC::clear_state();

    }

  }
}

int main() {
  MAILBOX::init();

  multicore_launch_core1(core1_main);
  // sleep_ms(10);
  core0_main();
  
} 


