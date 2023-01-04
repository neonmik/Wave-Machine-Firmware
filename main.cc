/**
 *            Beep Machine Firmware v0.13
 * 
 * Copyright (c) 2022 Nick Allott Musical Services (NAMS)
 *  
 */

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



#include "synth.h" // will remove/rework this eventually
#include "note_handling.cc"
#include "drivers/dac.h"
#include "hardware.h"

#define SAMPLE_RATE     44100

#define SONG_LENGTH 384
#define BPM 120 // part of a function for determinding bpm ms
#define HAT 20000 
#define BASS 500
#define SNARE 6000
#define SUB 50

using namespace synth;
using namespace beep_machine;

extern bool buffer_flag;
extern uint16_t play_buffer[256];


synth::AudioChannel synth::channels[MAX_VOICES];

void voices_init (void) {
  for (int i = 0; i < MAX_VOICES; i++) {
    channels[i].waveforms   = Waveform::SAW;
    channels[i].attack_ms   = 10;
    channels[i].decay_ms    = 10;
    channels[i].sustain     = 0xffff;
    channels[i].release_ms  = 10;
    channels[i].volume      = 10000;
    channels[i].off();
    channels[i].note        = 0;
    channels[i].frequency   = 0;
    channels[i].is_active   = 0;
  }
}

 int main() {
  
  uint8_t hardware_index;
  hardware_init();

  dac_init(SAMPLE_RATE);

  voices_init();

  

  while (true) {
    hardware_task();

    
    for (int i = 0; i < 8; i++) {
      channels[i].attack_ms = get_pagintaion(1,0);
      channels[i].decay_ms = get_pagintaion(1,1);
      channels[i].sustain = get_pagintaion(1,2);
      channels[i].release_ms = get_pagintaion(1,3);
    }
    if (buffer_flag){
      for(int i =0; i<256; i++) {
        play_buffer[i] = ((uint16_t) get_audio_frame());
		  // add counter here for playback/sequncer timing

      }
      buffer_flag = 0;
    }
  }
} 


