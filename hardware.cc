#include "hardware.h"
#include "log_table.h"

#include "drivers/adc.h"
#include "drivers/keys.h"
#include "drivers/button.h"
#include "drivers/leds.h"

#include "synth/modulation.h"
// #include "synth/arp.h"


extern uint16_t pitch_scale;
extern uint8_t hardware_index;

// uint32_t software_index = 0;


#define MAX_PRESETS     7

using namespace beep_machine;

extern void note_priority(int status, int note, int velocity);


Adc adc;
Keys keys;


// ----------------------
//          KEYS
// ----------------------


void keys_update() {
  uint32_t k, k_last;

	k =  keys.get();
	k_last = keys.get_last();
	// num_keys_down = 0;

	for (int i = 0; i < MAX_KEYS; i++) {
		// if ( !((k>>i) & 1) ) {
		// 	num_keys_down++;
		// }
		if ( (!((k>>i) & 1)) &&  (((k_last>>i) & 1))  )  {  // new key down
      // 
      if (KEYS_PRINT_OUT) printf("Key: %d on\n", ((i) + 1));
			note_priority(0x90, i+48, 127);   // keyboard starts at midi note 36
		}
		if ( ((k>>i) & 1) &&  (!((k_last>>i) & 1))  )  {  // key up

      note_priority(0x80, i+48 ,0);
      // set_note_off(i + 36);   // keyboard starts at midi note 36
			// dec_physical_notes_on();
		}
	}

  // Page
	if ( (!((k>>PAGE_KEY) & 1)) &&  (((k_last>>PAGE_KEY) & 1)) ){
    Buttons::PAGE.pressed();
    if (KEYS_PRINT_OUT) printf("Page key pressed\n");
	}
	if ( (((k>>PAGE_KEY) & 1)) &&  (!((k_last>>PAGE_KEY) & 1)) ){
    Buttons::PAGE.released();
    if (KEYS_PRINT_OUT) printf("Page key released\n");
	}

  // LFO
	if ( (!((k>>LFO_KEY) & 1)) &&  (((k_last>>LFO_KEY) & 1)) ){
    Buttons::LFO.pressed();
	}
  if ( (((k>>LFO_KEY) & 1)) &&  (!((k_last>>LFO_KEY) & 1)) ){
    Buttons::LFO.released();
	}

  // ARP
  if ( (!((k>>ARP_KEY) & 1)) &&  (((k_last>>ARP_KEY) & 1)) ){
    Buttons::ARP.pressed();
	} 
  if ( (((k>>ARP_KEY) & 1)) &&  (!((k_last>>ARP_KEY) & 1)) ){
    Buttons::ARP.released();
	}

  // Preset
  if ( (!((k>>PRESET_KEY) & 1)) &&  (((k_last>>PRESET_KEY) & 1)) ){
    Buttons::PRESET.pressed();
	}
	if ( (((k>>PRESET_KEY) & 1)) &&  (!((k_last>>PRESET_KEY) & 1)) ){
    Buttons::PRESET.released();
	}

	// store keys for next time
	keys.set_last(k);
}

// ----------------------
//          KNOBS
// ----------------------

void pagination_init() {
  // pinMode(aLED, OUTPUT);
  for(int i=0; i < MAX_KNOBS; i++){
    knob_values[i] = adc.value(i);
    knob_states[i] = ACTIVE;
  }
  default_pagination();
}
void default_pagination () {
  // ADSR default values
  page_values[Page::ADSR][0]=10; //A
  page_values[Page::ADSR][1]=20; // D
  page_values[Page::ADSR][2]=2047; // S
  page_values[Page::ADSR][3]=511; // R
  page_values[Page::LFO][0]=0; //A
  page_values[Page::LFO][1]=0; // D
  page_values[Page::LFO][2]=0; // S
  page_values[Page::LFO][3]=0; // R
}
// read knobs and digital switches and handle pagination
void pagination_update(){

  if(Buttons::PAGE.get_short()){
    uint8_t pages = MAX_PAGES;

    page_change = true;

    current_page++;

    // LFO on
    // if (!get_lfo_flag()) pages--;
    // ARP on
    if (!get_arp_flag()) pages--;
    
    // count the pages
    if (current_page >= pages) current_page = 0;
    
    set_page(current_page);
    set_page_flag(false);
  }

  // if page has changed then protect knobs
  if(page_change){
    page_change = false; // set the Page change back to false so it can be read again...

    // bitmask the knob leds off
    Leds::KNOBS_off();

    for(int i=0; i < MAX_KNOBS; i++){ // loop through the array and set all the values to protected.
      knob_states[i] = PROTECTED;
    }
  }
  // read knobs values, show sync with the LED, enable knob when it matches the stored value
  for (int i = 0; i < MAX_KNOBS; i++){
    value = adc.value(i);
    in_sync = abs(value - page_values[current_page][i]) < protection_value;

    // enable knob when it matches the stored value
    if (in_sync){
      knob_states[i] = ACTIVE;
    }
  
    // // if knob is moving, show if it's active or not
    // if (abs(value - knob_values[i]) > 5){
    //   // if knob is active, blink LED
    //   if(knob_states[i] == ACTIVE){
    //     // bitshift an ON to the current knob and output it to the ShiftReg
    //     sr_shift_out((leds |= 1 << (7 - i)));
    //   } else {
    //     // bitshift an OFF to the current knob and output it to the ShiftReg
    //     sr_shift_out((leds |= 0 << (7 - i)));
    //   }
    // }
    
    knob_values[i] = value;

    // if enabled then mirror the real time knob value
    if(knob_states[i] == ACTIVE){
      // Leds::SR.set_pin(i);
      Leds::KNOB_select(i);
      page_values[current_page][i] = value;
    }
  }
  pagination_flag = 1;
}

uint32_t beep_machine::get_pagintaion (int page, int knob) {
  return (page_values[page][knob]>>2);
}
uint8_t beep_machine::get_pagination_flag (void) {
  uint8_t temp;
  temp = pagination_flag;
  pagination_flag = 0;
  return temp;
}

void print_knob_array(uint32_t *array, int len){
  for(int i = 0;i< len;i++){
    printf("| ");
    printf("%.4d", array[i]);
  }
}
void print_knob_page(){
  printf("Page: %d ", current_page);
  print_knob_array(page_values[current_page], MAX_KNOBS);
  printf("\n");
}


// ----------------------
//          FLAGS
// ----------------------
void toggle_shift_flag (void) {
  shift_flag = !shift_flag;
}
bool get_shift_flag (void){
  return shift_flag;
}

void set_page (uint8_t value) {
  // using a switch here so that I can easily change the LEDs... find a better way?
  switch (value) {
    case 0:
      page = 0;
      Leds::PAGES_off();
      break;
    case 1:
      page = 1;
      Leds::PAGE_1.toggle();
      break;
    case 2:
      page = 2;
      Leds::PAGE_1.toggle();
      Leds::PAGE_2.toggle();
      break;
    case 3:
      page = 3;
      Leds::PAGE_2.toggle();
      Leds::PAGE_3.toggle();
      break;
  }
}
uint8_t get_page(void) {
  return page;
}
void set_page_flag(uint8_t value) {
  page_flag = value;
}
uint8_t get_page_flag(void) {
  return page_flag;
}

void set_lfo_flag(uint8_t value) {
  lfo_flag = value;
  if (lfo_flag) Leds::LFO.on();
  if (!lfo_flag) Leds::LFO.off();

}
void toggle_lfo_flag(void) {
  modulation::toggle();

  Leds::LFO.toggle();
  lfo_flag = !lfo_flag;
  if (KEYS_PRINT_OUT) printf("Key: LFO\n");
}
uint8_t get_lfo_flag(void) {
  return lfo_flag;
}

void set_arp_flag(uint8_t value) {
  arp_flag = value;
  if (arp_flag) Leds::ARP.on();
  if (!arp_flag) Leds::ARP.off();
}
void toggle_arp_flag(void) {
  arp_flag = !arp_flag;
  Leds::ARP.toggle();
  if (KEYS_PRINT_OUT) printf("Key: ARP\n");
}
uint8_t get_arp_flag(void) {
  return arp_flag;
}

void set_preset(uint8_t value) {
  preset = value;
}
void change_preset(void) {
  preset++;
  preset&=0x7;
  set_preset(preset);
  Leds::RGB.preset(preset);
}
uint8_t get_preset(void) {
  return preset;
}
void set_preset_flag(uint8_t value) {
  preset_flag = value;
}
uint8_t get_preset_flag(void) {
  return preset_flag;
}


// ----------------------
//        HARDWARE
// ----------------------


void hardware_init (void) {
  stdio_init_all();

  Leds::init();
  keys.init();
  adc.init();
  pagination_init();
  

  puts("Welcome to the jungle...");

  if (HARDWARE_TEST) hardware_test(50);

  hardware_index = 0;
}

void hardware_test (int delay) {
  
  Leds::test(delay);
}

void hardware_task (void) {
  if (hardware_index == 0) {
    keys.read();
  }
  if (hardware_index == 1) {
    keys_update();
    if (Buttons::ARP.get_short()) toggle_arp_flag();
    if (Buttons::PRESET.get_short()) change_preset();
    if (Buttons::LFO.get_short()) toggle_lfo_flag();
  }
  if (hardware_index == 2) {
    adc.update();
  }
  if (hardware_index == 3) {

    Leds::update();
  }
  if (hardware_index == 4) {
    pagination_update();
  }
  if (hardware_index == 5) {
    // if (page_button.get_shift()) {
          
    // }
    switch (get_page()) {
      case 0:
        // --------------- //
        // PAGE 0 (GLOBAL) //
        // --------------- //

        synth::wave = ((get_pagintaion(0,0)>>6)*256);
        synth::wave_vector = (get_pagintaion(0,1));
        // 3
        pitch_scale = get_pitch_log(get_pagintaion(0,3)); // might need optional stability/lofi switch...
        break;
      case 1:
        // ------------- //
        // PAGE 1 (ADSR) //
        // ------------- //
        
        // need to implement an easy change thing... knobs_touched?
        // if (get_pagination_flag()) {
        // 1 - ATTACK
        synth::attack_ms = ((get_pagintaion(1,0)+10)<<2);
        // 2 - DECAY
        synth::decay_ms = ((get_pagintaion(1,1)+10)<<2); // +10 because if it geos below roughly there, the note volume is unpredicatble... theres also an issue between attack and decay levels outputing something weird
        // 3 - SUSTAIN
        synth::sustain = (get_pagintaion(1,2)<<6);
        // 4 - RELEASE
        synth::release_ms = ((get_pagintaion(1,3))<<2);
        break;
      case 2:
        // ----------------------- //
        // PAGE 2 (LFO)            //
        // ----------------------- //
        // 1 - MATRIX
        // uint8_t tester = (get_pagintaion(2,0)>>8);
        modulation::set_matrix(get_pagintaion(2,0)>>8);
        // 2 - RATE
        modulation::set_rate(get_pagintaion(2,1));
        // 3 - DEPTH
        modulation::set_depth(get_pagintaion(2,2));
        // 4 - WAVESHAPE
        modulation::set_wave(get_pagintaion(2,3));
        break;
      case 3:
        // ----------------------------------- //
        // ??? PAGE 3 (ARP) (Optional)         //
        // ----------------------------------- //
        // 1 - MATRIX
        // arpDelay = ((pageValues[2][0]>>1) + 1); // need the +1 to keep the timer going
        // 2
        // arpRelease = map(pageValues[2][1], 0, 1023, 40, 300); // 40 lets you get down to blips and boops, not sure this really is useful... 
        // 3
        // ??? ???
        // 4
        // ??? ???
        break;
    }
  }


  if (KNOBS_PRINT_OUT) {
    if (hardware_index==200) {
      print_knob_page();
      }
  }

}
void hardware_debug (void) {
  Leds::SPARE.toggle();
}