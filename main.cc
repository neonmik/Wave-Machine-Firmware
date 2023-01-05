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
uint8_t hardware_index;

synth::AudioChannel synth::channels[MAX_VOICES];

void voices_init (void) {
  for (int i = 0; i < MAX_VOICES; i++) {
    channels[i].waveforms   = Waveform::SINE;
    channels[i].attack_ms   = 1000;
    channels[i].decay_ms    = 100;
    channels[i].sustain     = 0xfff; // full scale is 0xffff = 65535 (but only goes to 65534?)
    channels[i].release_ms  = 1000;
    channels[i].volume      = 0x7fff;
    // channels[i].off();
    channels[i].note        = 0;
    channels[i].frequency   = 0;
    channels[i].is_active   = 0;
  }
}

int main() {
  
  
  hardware_init();

  dac_init(SAMPLE_RATE);

  voices_init();

  

  while (true) {
    
    // gpio_put(LED_ARP_PIN, 0);
    // hardware_task();
    if (buffer_flag==0) {
      hardware_task();
      // 
    }
    // if (get_pagination_flag()) {
    //   for (int i = 0; i < 8; i++) {
    //     channels[i].attack_ms = (get_pagintaion(1,0)<<2);
    //     channels[i].decay_ms = (get_pagintaion(1,1)<<2);
    //     channels[i].sustain = (get_pagintaion(1,2)<<6);
    //     channels[i].release_ms = (get_pagintaion(1,3)<<2);
    //   }
    // }
    
    // hardware_task();
    if (buffer_flag){
      
      for(int i =0; i<256; i++) {
        // play_buffer[i] = 0;
        uint16_t sample = ((get_audio_frame()+32768)>>4);
        play_buffer[i] = sample;
        // printf("%.5u \n", play_buffer[i]);
		  // add counter here for playback/sequncer timing

      }
    // printf("hardware_index: %d \n", hardware_index);
      // hardware_index = 0;
      buffer_flag = 0;
      
    }
    
    // printf("ADSR: %.8d \n", );
    

    hardware_index++;
    hardware_index&=0xff;
    // gpio_put(LED_ARP_PIN, 1);
  }
} 


