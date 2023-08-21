/**
 *                Wave Machine Firmware v0.29
 * 
 * Copyright (c) 2022-2023 Nick Allott Musical Services (NAMS)
 *  
 */


#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "config.h"
#include "debug.h"

#include "synth/arp.h"
#include "synth/synth.h"

#include "drivers/dac.h"

#include "ui.h"

#include "queue.h"

uint8_t voices_active;

void hw_core() {
  
  UI::Init();

  while (true) {

    UI::Update();

  }
}


void synth_core() {

  SYNTH::Init();
  DAC::Init(SYNTH::get_audio_frame);
  CLOCK::Init();
  
  while (true) {
     if (DAC::get_state()) {
      // tidy this...
      uint8_t temp = QUEUE::trigger_check_queue();
      if (temp) {
        for (int i = 0; i < temp; i++){
          uint8_t slot_;
          uint8_t note_;
          bool gate_;

          QUEUE::trigger_receive(slot_, note_, gate_);
          if (slot_ == FILTER_VOICE) {
            if (gate_) FILTER::trigger_attack();
            else FILTER::trigger_release();
          } 
          else {
            if (gate_) {
              SYNTH::voice_on(slot_, note_);
            } else {
              SYNTH::voice_off(slot_);
            }
            if (slot_ > POLYPHONY) printf("Voice Queue error! Out of bounds voice\n");
          }
        }
      }

      DAC::clear_state();
    }
  }
}

 int main() {

  set_sys_clock_khz(CORE_SPEED, true); // needs to be called before UART. Extra speed is needed to run the code.

  stdio_init_all(); // has to be here to allow both cores to use the debug serial UART. 

  QUEUE::Init(); // has to be here to allow both cores access to QUEUE
  
  multicore_launch_core1(hw_core); // launches the hardware core

  synth_core(); // launches the synth/audio core

} 

