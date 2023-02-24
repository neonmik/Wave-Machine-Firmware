/**
 *            Beep Machine Firmware v0.23
 * 
 * Copyright (c) 2022 Nick Allott Musical Services (NAMS)
 *  
 */


#include <stdio.h>

#include "pico/stdlib.h"

#include "synth/modulation.h"
#include "synth/arp.h"
#include "synth/synth.h"

#include "drivers/dac.h"

#include "ui.h"

#define SAMPLE_RATE     44100
#define MAX_VOICES      8
#define BPM             120 // part of a function for determinding bpm ms


int main() {
  
  
  UI::init();
  

  SYNTH::init(SAMPLE_RATE);
  MOD::init();
  ARP::init(BPM, SAMPLE_RATE);

  DAC::init(SAMPLE_RATE);

  while (true) {

    if (DAC::get_state()){
      
      if (SETTINGS::get_lfo()) MOD::update(); // only updates the MOD values every 256 samples

      for(int i = 0; i < 256; i++) {
        DAC::fill(SYNTH::get_audio_frame(), i);
        ARP::index();
      }

      DAC::clear_state();
      continue; // skips the UI update to save resources
    } 
    
    else {
      UI::update();

    }

  }
} 


