/**
 *            Beep Machine Firmware v0.23
 * 
 * Copyright (c) 2022 Nick Allott Musical Services (NAMS)
 *  
 */


// notes:
//        - added ADSR controls - not working.
//        - moved ADSR & Waveforms so they're global - didnt work, sacked off Waveforms, and ADSR isnt right... need to nail down my transfer of values from the hardware to the software
//        - tried to extern sample_rate to main - didnt work

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/time.h"

#include "hardware/adc.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

#include <math.h> 


#include "synth/modulation.h"
#include "synth/arp.h"
#include "synth/synth.h"
#include "note_priority.cc"
#include "drivers/dac.h"
#include "hardware.h"


#define SAMPLE_RATE     44100
#define MAX_VOICES      8
#define BPM 120 // part of a function for determinding bpm ms



extern bool buffer_flag;
extern uint16_t play_buffer[256];
uint8_t hardware_index;
uint16_t software_index = 0;
extern uint16_t Arp::beat;


  int main() {
  
  
  hardware_init();

  dac_init(SAMPLE_RATE);

  modulation::init();
  Arp::init(BPM, SAMPLE_RATE);

  while (true) {
    
    if (buffer_flag){

      modulation::update();
      for(int i =0; i<256; i++) {
        uint16_t sample = ((synth::get_audio_frame()+32767)>>4); // create next sample, add 32767 (to move from a signed to unsigned, deafult C behavior is wrong), then shift down to 12 bit from 16 bit.
        play_buffer[i] = sample;

        // ounter here for playback/sequncer timing
        software_index++;
        Arp::update_playback();
      }
      buffer_flag = 0;
      // hardware_debug();
      continue;

    } else {
        
      hardware_task();
        
      hardware_index++;
      //could be either of these, but apprently you can't loop 4/5/6 times like this...?
      hardware_index &= 0x7;
      if (hardware_index > 5) hardware_index = 0; // this takes 2-3 more instructions to accomplish

    }

    
    

  }
} 


