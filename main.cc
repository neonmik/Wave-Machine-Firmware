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

#include "../synth/clock.h"

#include "ui.h"

#include "queue.h"

bool      isBufferFull;

extern uint32_t    sample_clock;
extern uint8_t     softwareIndex;
extern uint8_t     hardwareIndex;
extern uint16_t    playBuffer[];


void hw_core() {
  
  UI::Init();

  while (true) {

    UI::Update();

  }
}


void synth_core() {

  SYNTH::Init();
  DAC::Init(SYNTH::process);
  CLOCK::Init();
  
  while (true) {
    if (softwareIndex != hardwareIndex) {
      playBuffer[softwareIndex] = SYNTH::process();

      ++softwareIndex;
      softwareIndex &= 0xff;

      ++sample_clock;

      CLOCK::tick();
    }


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
  
  multicore_launch_core1(synth_core); // launches the 2nd core

  // synth_core(); // launches the synth/audio core

  hw_core(); // launch the hardware core

} 

