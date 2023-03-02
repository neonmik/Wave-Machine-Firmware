/**
 *            Beep Machine Firmware v0.23
 * 
 * Copyright (c) 2022 Nick Allott Musical Services (NAMS)
 *  
 */


#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "synth/modulation.h"
#include "synth/arp.h"
#include "synth/synth.h"

#include "drivers/dac.h"

#include "ui.h"

#define SAMPLE_RATE     44100
#define MAX_VOICES      8
#define BPM             120 // part of a function for determinding bpm ms

uint32_t software_index;

void core1_entry() {
  DAC::init(SAMPLE_RATE, SYNTH::get_audio_frame);
  SYNTH::init(SAMPLE_RATE);
}

int main() {

  
  UI::init();
  
  SYNTH::init(SAMPLE_RATE);
  MOD::init();
  ARP::init(BPM, SAMPLE_RATE);

  DAC::init(SAMPLE_RATE, SYNTH::get_audio_frame);
  // multicore_launch_core1(core1_entry);

  while (true) {
    
    if (DAC::get_state()){
      if (SETTINGS::get_lfo()) MOD::update(); // only updates the MOD values every 64 samples
      
      DAC::clear_state();
      continue; // skips the UI update to save resources
    } 
    
    else {
      UI::update();
    }

  }
} 


