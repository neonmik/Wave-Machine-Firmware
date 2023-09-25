/**
 *                Wave Machine Firmware v0.31
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

#include "synth/clock.h"

#include "ui.h"

#include "queue.h"

// bool      isBufferFull;

extern uint32_t    sampleClock;
uint32_t           sampleClockLast;
extern uint8_t     softwareIndex;
extern uint8_t     hardwareIndex;
extern uint16_t    playBuffer[];


void hardwareCore() {
  
  UI::init();

  while (true) {

    UI::update();

  }
}


void synthCore() {

  SYNTH::init();
  DAC::init();
  CLOCK::init();
  
  while (true) {
    if (softwareIndex != hardwareIndex) {
      playBuffer[softwareIndex] = SYNTH::process();

      ++softwareIndex;
      softwareIndex &= 0x1F; // loops the play buffer every 32 samples

      ++sampleClock;

      CLOCK::sampleClockTick();
    }

    if ((!(sampleClock & 0x3F)) && (sampleClock != sampleClockLast)){
			// make sure this only happens once every 64 sample periods
			sampleClockLast = sampleClock;

      if (QUEUE::triggerCheckQueue()) {
        uint8_t slot, note;
        bool gate;

        while (QUEUE::triggerReceive(slot, note, gate)) {
          if (gate) {
            SYNTH::voiceOn(slot, note);
          } else {
            SYNTH::voiceOff(slot);
          }
          if (slot > POLYPHONY) DEBUG::error("Out of bounds voice!");
        }
      }
    }
  }
}

 int main() {

  set_sys_clock_khz(CORE_SPEED, true); // needs to be called before UART. Extra speed is needed to run the code.

  stdio_init_all(); // has to be here to allow both cores to use the debug serial UART. 

  QUEUE::init(); // has to be here to allow both cores access to QUEUE
  
  // multicore_launch_core1(hardwareCore); // launches the 2nd core
  // synthCore(); // launches the synth/audio core

  multicore_launch_core1(synthCore); // launches the 2nd core
  hardwareCore(); // launch the hardware core

} 

