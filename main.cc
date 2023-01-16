/**
 *            Beep Machine Firmware v0.13
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
#include "synth.h"
#include "note_priority.cc"
#include "drivers/dac.h"
#include "hardware.h"


#define SAMPLE_RATE     44100
#define BPM 120 // part of a function for determinding bpm ms



using namespace synth;
synth::AudioChannel synth::channels[MAX_VOICES];
extern uint16_t synth::attack_ms;      // attack period - moved to global as it's not needed per voice for this implementation.
extern uint16_t synth::decay_ms;      // decay period
extern uint16_t synth::sustain;   // sustain volume
extern uint16_t synth::release_ms;      // release period
extern uint8_t synth::waveforms;      // bitmask for enabled waveforms (see AudioWaveform enum for values)

using namespace beep_machine;


extern bool buffer_flag;
extern uint16_t play_buffer[256];
uint8_t hardware_index;




void voices_init (void) {
  
  synth::attack_ms   = 1000; 
  synth::decay_ms    = 100;
  synth::sustain     = 0x7fff; // set to less than full scale as it clips - full scale: 0xffff = 65535 
  synth::release_ms  = 1000;
  synth::waveforms = Waveform::WAVETABLE;      // bitmask for enabled waveforms (see AudioWaveform enum for values)

  for (int i = 0; i < MAX_VOICES; i++) {
    // channels[i].waveforms   = Waveform::SINE | Waveform::SAW; 
    channels[i].volume      = 0x7fff; // halved channel volume so it doesnt clip - full scale : 0xffff = 65535
    channels[i].note        = 0;
    channels[i].frequency   = 0;
    channels[i].is_active   = 0;
  }
}

int main() {
  
  
  hardware_init();

  dac_init(SAMPLE_RATE);

  voices_init();
  modulation::init();

  while (true) {
    
    if (buffer_flag){
      if (get_lfo_flag()) modulation::update();
      // modulation::update();
      synth::wave_vector += (modulation::get_output_uint()>>4);
      for(int i =0; i<256; i++) {
        uint16_t sample = ((get_audio_frame()+32768)>>4); // create next sample, add 32768 (to move from a signed to unsigned, deafult C behavior is wrong), then shift down to 12 bit from 16 bit.
        play_buffer[i] = sample;
		  // add counter here for playback/sequncer timing

      }
      buffer_flag = 0;

    } else {
      hardware_task();
    }
    // gpio_put(DEBUG_PIN, 1);
    
    
    
    // printf("release: %.4d \n", synth::release_ms);
    

    hardware_index++;
    hardware_index&=0xff;

    // gpio_put(DEBUG_PIN, 0);
  }
} 


