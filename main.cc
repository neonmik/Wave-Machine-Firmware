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

  while (true) {

    UI::update();

  }
}


void core0_main() {

  SYNTH::init(SAMPLE_RATE);
  MOD::init(SAMPLE_RATE);
  ARP::init(BPM, SAMPLE_RATE);
  DAC::init(SAMPLE_RATE, SYNTH::get_audio_frame);

  while (true) {
    // once the buffer is full, update the timing critical stuff first, then the everything else.
    if (DAC::get_state()) {
      
      MAILBOX::receive(); //copy the data from the mailbox to the local variables

      NOTE_PRIORITY::update(); // update notes from the mailbox info
      SYNTH::update(); // update the controls for the synth
      if (SETTINGS::get_lfo()) MOD::update(); // update the modulation if it is enabled

      DAC::clear_state();
    }

  }
}

int main() {
  MAILBOX::init();

  multicore_launch_core1(core0_main);

  core1_main();
  
} 


