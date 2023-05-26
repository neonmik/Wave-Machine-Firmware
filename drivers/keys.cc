#include "keys.h"


#include "../synth/arp.h"


namespace KEYS {
  
  Keyboard Keys;

  void Keyboard::init(){
    // initiate pins for mux address
    gpio_init(MUX_SEL_A);
    gpio_init(MUX_SEL_B);
    gpio_init(MUX_SEL_C);
    gpio_init(MUX_SEL_D);
    // set the pins direction
    gpio_set_dir(MUX_SEL_A, GPIO_OUT);
    gpio_set_dir(MUX_SEL_B, GPIO_OUT);
    gpio_set_dir(MUX_SEL_C, GPIO_OUT);
    gpio_set_dir(MUX_SEL_D, GPIO_OUT);
    // set the slew rate slow (for reducing amount of cross talk on address changes... hopefully)
    gpio_set_slew_rate(MUX_SEL_A, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(MUX_SEL_B, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(MUX_SEL_C, GPIO_SLEW_RATE_SLOW);
    gpio_set_slew_rate(MUX_SEL_D, GPIO_SLEW_RATE_SLOW);

    // printf("MUX_A: %u\n", (uint8_t)gpio_get_drive_strength(MUX_SEL_A));
    // printf("MUX_B: %u\n", (uint8_t)gpio_get_drive_strength(MUX_SEL_B));
    // printf("MUX_C: %u\n", (uint8_t)gpio_get_drive_strength(MUX_SEL_C));
    // printf("MUX_D: %u\n", (uint8_t)gpio_get_drive_strength(MUX_SEL_D));

    // test lowering the drive strength
    // gpio_set_drive_strength(MUX_SEL_A, GPIO_DRIVE_STRENGTH_2MA);
    // gpio_set_drive_strength(MUX_SEL_B, GPIO_DRIVE_STRENGTH_2MA);
    // gpio_set_drive_strength(MUX_SEL_C, GPIO_DRIVE_STRENGTH_2MA);
    // gpio_set_drive_strength(MUX_SEL_D, GPIO_DRIVE_STRENGTH_2MA);

    // initiate pins for mux output
    gpio_init(MUX_OUT_0);
    gpio_init(MUX_OUT_1);
    // set the pins direction
    gpio_set_dir(MUX_OUT_0, GPIO_IN);
    gpio_set_dir(MUX_OUT_1, GPIO_IN);
    // set pins for pull up - already doing this on PCB, just a precaution
    gpio_pull_up(MUX_OUT_0);
    gpio_pull_up(MUX_OUT_1);

    gpio_put(MUX_SEL_A, 1);
    gpio_put(MUX_SEL_B, 1);
    gpio_put(MUX_SEL_C, 1);
    gpio_put(MUX_SEL_D, 1);
  }
  void Keyboard::read(){
    
    _history[history_index] = 0;
    
    gpio_put(MUX_SEL_A, 0);
    gpio_put(MUX_SEL_B, 0);
    gpio_put(MUX_SEL_C, 0);
    gpio_put(MUX_SEL_D, 0); 

    for (int i = 0; i < 16; i++) {

      // nop needed to stop the processor reading too quickly, the mux cant keep up...

      asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
      gpio_put(MUX_SEL_A, i & 1); 
      asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
      gpio_put(MUX_SEL_B, i & 2);
      asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
      gpio_put(MUX_SEL_C, i & 4);
      asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
      gpio_put(MUX_SEL_D, i & 8);
        
      asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
      
      _history[history_index] |= gpio_get(MUX_OUT_0) << (i);
      asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
      _history[history_index] |= gpio_get(MUX_OUT_1) << (i + 16);
      
      
    }
    // masks 32th key (spare pin in) and function keys - saves pulling floating pins high
    _history[history_index] |= 0x1; 

    // loops round history index
    history_index++;
    history_index &= 0x7;
    
    if ((_history[0] == _history[1]) &&
      (_history[0] == _history[2]) &&
      (_history[0] == _history[3]) &&
      (_history[0] == _history[4]) &&
      (_history[0] == _history[5]) &&
      (_history[0] == _history[6]) &&
      (_history[0] == _history[7])) {
        // reverse bit order  - for messed up hardware
        _current = reverse(_history[0]);
    }

  }

  void init () {
    // initiate GPIO
    Keys.init();

    // read the keys 8 times round to prime for the debounce
    for (int i = 0; i < 8; i++) {
      Keys.read();
    }
    // update before leaving initiation (needs to happen for start up settings)
    update();
  }

  void read () {
    Keys.read();
  }
  
  void update () {
    uint32_t keys, keys_last;

    keys = Keys.get();
    keys_last = Keys.get_last();

    for (int i = 0; i < MAX_KEYS; i++) {
      if ( (!((keys>>i) & 1)) &&  (((keys_last>>i) & 1))  )  {  // new key down
        note_on(i+48);
        MIDI::sendNoteOn(i+48, 127);
        _changed = true;
      }
      if ( ((keys>>i) & 1) &&  (!((keys_last>>i) & 1))  )  {  // key up
        note_off(i+48);
        MIDI::sendNoteOff(i+48, 0);
        _changed = true;
      }
    }
    if (_changed) MAILBOX::NOTE_DATA.send();


    // Page
    if ( (!((keys>>PAGE_KEY) & 1)) &&  (((keys_last>>PAGE_KEY) & 1)) ){
      Buttons::PAGE.pressed();
    }
    if ( (((keys>>PAGE_KEY) & 1)) &&  (!((keys_last>>PAGE_KEY) & 1)) ){
      Buttons::PAGE.released();
    }

    // LFO
    if ( (!((keys>>LFO_KEY) & 1)) &&  (((keys_last>>LFO_KEY) & 1)) ){
      Buttons::LFO.pressed();
    }
    if ( (((keys>>LFO_KEY) & 1)) &&  (!((keys_last>>LFO_KEY) & 1)) ){
      Buttons::LFO.released();
    }

    // ARP
    if ( (!((keys>>ARP_KEY) & 1)) &&  (((keys_last>>ARP_KEY) & 1)) ){
      Buttons::ARP.pressed();
    } 
    if ( (((keys>>ARP_KEY) & 1)) &&  (!((keys_last>>ARP_KEY) & 1)) ){
      Buttons::ARP.released();
    }

    // Preset
    if ( (!((keys>>PRESET_KEY) & 1)) &&  (((keys_last>>PRESET_KEY) & 1)) ){
      Buttons::PRESET.pressed();
    }
    if ( (((keys>>PRESET_KEY) & 1)) &&  (!((keys_last>>PRESET_KEY) & 1)) ){
      Buttons::PRESET.released();
    }

    // store keys for next time
    Keys.set_last(keys);
    
  }

  void note_on (uint8_t note) {
    NOTES.note_state[note & 127] = 1;
    inc_physical_notes();
  }
  void note_off (uint8_t note) {
    NOTES.note_state[note & 127] = 0;
    dec_physical_notes();
  }
  void notes_clear (void) {
    for (int i = 0; i < 128; i++) {
      NOTES.note_state[i] = 0;
    }
    NOTES.notes_on = 0;
  }
  uint8_t get_notes_on (void) {
    return NOTES.notes_on;
  }
}
