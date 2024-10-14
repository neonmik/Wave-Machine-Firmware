/**
 *                Wave Machine Firmware v0.32
 * 
 * Copyright (c) 2022-2023 Nick Allott Musical Services (NAMS)
 *  
 */


#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/stdio_uart.h"

#include "config.h"
#include "debug.h"

#include "synth/arp.h"
#include "synth/synth.h"

#include "drivers/dac.h"

#include "synth/clock.h"

#include "ui.h"

#include "queue.h"
#include "interval.h"

extern uint32_t    sampleClock;
uint32_t           sampleClockLast;
extern uint8_t     softwareIndex;
extern uint8_t     hardwareIndex;
extern volatile uint16_t    playBuffer[];

extern bool        startUpComplete;
extern bool        calibrationCheck;


void hardwareCore() {
  
  UI::init();

  while (true) {

    UI::update();
  }
}


void synthCore() {


  printf("\n\nWaiting for UI to start up");
  while (!startUpComplete) {
    sleep_ms(250);
    if (!calibrationCheck) printf(".");
  }
  printf("\n\nAudio core starting");

  SYNTH::init();
  DAC::init();
  CLOCK::init();
  
  printf("!\n");

  while (true) {
    if (softwareIndex != hardwareIndex) {
      
      playBuffer[softwareIndex] = SYNTH::process();

      ++softwareIndex;
      softwareIndex %= 32; // loops the play buffer every 32 samples

      ++sampleClock;

      CLOCK::internalClockTick();
    }

    if (!softwareIndex) {
      SYNTH::calculateIncrements();
    }

    if (INTERVAL::QUEUE.checkInterval()) {
      QUEUE::update();
    } 
  }
}

int main() {

  // set_sys_clock_khz(CORE_SPEED, true); // needs to be called before UART. Extra speed is needed to run the code.

  stdio_init_all(); // has to be here to allow both cores to use the debug serial UART. 

  QUEUE::init(); // has to be here to allow both cores access to QUEUE

  multicore_launch_core1(synthCore); // launches the 2nd core
  hardwareCore(); // launch the hardware core
} 


