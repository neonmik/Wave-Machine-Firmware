#include "keys.h"


// #include "../synth/arp.h"


namespace KEYS {
  void init () {
    pinInit(MUX_OUT_0);
    pinInit(MUX_OUT_1);
  }

  void read () {
    uint8_t address = MUX::getAddress();

    if (address == lastAddress) return;

      
    readBuffer |= gpio_get(MUX_OUT_0) << (address);
    readBuffer |= gpio_get(MUX_OUT_1) << (address + 16);
    
    lastAddress = address;

    if (address == 15) {
      // if the address has done a full loop, prepare the history for a new read

      // masks 32th key (spare pin in) and function keys - saves pulling floating pins high
      readBuffer |= 0x1; // Currently 0b00000000000000000000000000000001 but will need to be swapped to 0b10000000000000000000000000000000 due to the hardware design fault.
      
      current = reverse(readBuffer);

      readBuffer = 0;
    }
  }
  
  void update () {

    if (current == last) return;

    for (int i = 0; i < MAX_KEYS; i++) {
      if ( (!((current>>i) & 1)) &&  (((last>>i) & 1))  )  {  // new key down
        NOTE_HANDLING::noteOn(i+DEFAULT_KEY, DEFAULT_ON_VEL);
      }
      if ( ((current>>i) & 1) &&  (!((last>>i) & 1))  )  {  // key up
        NOTE_HANDLING::noteOff(i+DEFAULT_KEY, DEFAULT_OFF_VEL);
      }
    }

    // Page
    if ( (!((current>>PAGE_KEY) & 1)) &&  (((last>>PAGE_KEY) & 1)) ){
      Buttons::PAGE.pressed();
    }
    if ( (((current>>PAGE_KEY) & 1)) &&  (!((last>>PAGE_KEY) & 1)) ){
      Buttons::PAGE.released();
    }

    // LFO
    if ( (!((current>>LFO_KEY) & 1)) &&  (((last>>LFO_KEY) & 1)) ){
      Buttons::FUNC1.pressed();
    }
    if ( (((current>>LFO_KEY) & 1)) &&  (!((last>>LFO_KEY) & 1)) ){
      Buttons::FUNC1.released();
    }

    // ARP
    if ( (!((current>>ARP_KEY) & 1)) &&  (((last>>ARP_KEY) & 1)) ){
      Buttons::FUNC2.pressed();
    } 
    if ( (((current>>ARP_KEY) & 1)) &&  (!((last>>ARP_KEY) & 1)) ){
      Buttons::FUNC2.released();
    }

    // Preset
    if ( (!((current>>PRESET_KEY) & 1)) &&  (((last>>PRESET_KEY) & 1)) ){
      Buttons::PRESET.pressed();
    }
    if ( (((current>>PRESET_KEY) & 1)) &&  (!((last>>PRESET_KEY) & 1)) ){
      Buttons::PRESET.released();
    }

    // store keys for next time
    setLast(current);
    
  }

  void printBuffer(void) {
    for (int i = 0; i < 32; i++) {
      printf("%d", (current>>i) & 1);
    }
    printf("\n");
  }
}
