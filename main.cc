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

#include <math.h> 

#include "synth.h" // will remove/rework this eventually
#include "note_handling.cc"
#include "dac.h"
#include "hardware.h"



#define SONG_LENGTH 384
#define BPM 120 // part of a function for determinding bpm ms
#define HAT 20000 
#define BASS 500
#define SNARE 6000
#define SUB 50

using namespace synth;

extern bool buffer_flag;
extern uint16_t play_buf[256];

synth::AudioChannel synth::channels[CHANNEL_COUNT];

const int16_t notes[5][SONG_LENGTH] = {
  { // melody notes
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 220, 0, 196, 0, 147, 0, 175, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 220, 0, 196, 0, 147, 0, 175, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
    147, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 247, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 175, 0, 0, 0, 0, 0, 0, 0, 175, 0, 196, 0, 220, 0, 262, 0, 330, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 349, 0, 0, 0, 0, 0, 0, 0, 349, 0, 330, 0, 294, 0, 220, 0, 262, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 247, 0, 0, 0, 0, 0, 0, 0, 247, 0, 262, 0, 294, 0, 392, 0, 440, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  },
  { // rhythm notes
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0,
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0,
    294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 294, 0, 440, 0, 587, 0, 440, 0, 392, 0, 523, 0, 659, 0, 523, 0, 392, 0, 523, 0, 659, 0, 523, 0, 698, 0, 587, 0, 440, 0, 587, 0, 698, 0, 587, 0, 440, 0, 587, 0, 523, 0, 440, 0, 330, 0, 440, 0, 523, 0, 440, 0, 330, 0, 440, 0, 349, 0, 294, 0, 220, 0, 294, 0, 349, 0, 294, 0, 220, 0, 294, 0, 262, 0, 247, 0, 220, 0, 175, 0, 165, 0, 147, 0, 131, 0, 98, 0,
  },
  { // drum beats
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, 
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, 
    BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, BASS, -1, BASS, -1, 0, 0, 0, 0, 0, 0, SNARE, 0, -1, 0, 0, 0, 0, 0
  },
  { // hi-hat
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, 
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, 
    HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1, HAT, -1
  },
  { // bass notes under bass drum
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 
    SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, SUB, -1, SUB, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0
  }, 
};
void update_playback(void) {
  static uint16_t prev_beat = 1;
  static uint16_t beat = 0;
  static uint16_t bpm_to_ms = 0;

  absolute_time_t at = get_absolute_time();
  uint64_t tick_ms = to_us_since_boot(at) / 1000;

  bpm_to_ms = 60,000/BPM;// 60,000ms / 120bpm = 500ms (1/4 note)... 
  beat = (tick_ms / (bpm_to_ms/4)) % SONG_LENGTH; // beat = 500ms/4 = 125ms (1/16th note)

  if (beat == prev_beat) return;
  prev_beat = beat;

  for(uint8_t i = 0; i < 5; i++) {
    if(notes[i][beat] > 0) {
      channels[i].frequency = notes[i][beat];
      channels[i].trigger_attack();
    } else if (notes[i][beat] == -1) {
      channels[i].trigger_release();
    }
  }
}
void song_init (void){
    // melody
  channels[0].waveforms   = Waveform::TRIANGLE | Waveform::SQUARE;
  channels[0].attack_ms   = 16;
  channels[0].decay_ms    = 168;
  channels[0].sustain     = 0xafff;
  channels[0].release_ms  = 168;
  channels[0].volume      = 100;

  // rhythm track
  channels[1].waveforms   = Waveform::SINE | Waveform::SQUARE;
  channels[1].attack_ms   = 38;
  channels[1].decay_ms    = 300;
  channels[1].sustain     = 0;
  channels[1].release_ms  = 0;
  channels[1].volume      = 120;

  // drum track
  channels[2].waveforms   = Waveform::NOISE;
  channels[2].attack_ms   = 5;
  channels[2].decay_ms    = 10;
  channels[2].sustain     = 16000;
  channels[2].release_ms  = 100;
  channels[2].volume      = 180;

  // hi-hat track
  channels[3].waveforms   = Waveform::NOISE;
  channels[3].attack_ms   = 5;
  channels[3].decay_ms    = 5;
  channels[3].sustain     = 8000;
  channels[3].release_ms  = 40;
  channels[3].volume      = 80;

  // bass track
  channels[4].waveforms   = Waveform::SQUARE;
  channels[4].attack_ms   = 10;
  channels[4].decay_ms    = 100;
  channels[4].sustain     = 0;
  channels[4].release_ms  = 500;
  channels[4].volume      = 120;

}

void voices_init (void) {
  for (int i = 0; i < CHANNEL_COUNT; i++) {
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
void voices_set (int a, int d, int s, int r, int w) {
  for (int i = 0; i < CHANNEL_COUNT; i++) {
    channels[i].waveforms   = w;
    channels[i].attack_ms   = a<<4;
    channels[i].decay_ms    = d<<4;
    channels[i].sustain     = s<<4;
    channels[i].release_ms  = r<<4;
  }
}

 int main() {
  
  uint8_t hardware_index;
  hardware_init();

  dac_init();

  voices_init();

  while (true) {
    hardware_task();
    if (buffer_flag){
      for(int i =0; i<256; i++) {
        play_buf[i] = ((uint16_t) get_audio_frame());
		  // add counter here for playback/sequncer timing
		  // update_playback();
      }
      printf("adsr-phase: %d \n", channels[0].adsr);
      buffer_flag = 0;
    }
  }
} 


