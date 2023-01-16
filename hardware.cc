#include "hardware.h"
#include "log_table.h"

#include "drivers/adc.h"
#include "drivers/keys.h"

#include "synth/modulation.h"


extern uint16_t pitch_scale;
extern uint8_t hardware_index;

uint32_t software_index = 0;


#define MAX_PRESETS     7

using namespace beep_machine;

extern void note_priority(int status, int note, int velocity);

extern uint8_t synth::waveforms;      // bitmask for enabled waveforms (see AudioWaveform enum for values)

extern uint16_t  synth::attack_ms;      // attack period - moved to global as it's not needed per voice for this implementation.
extern uint16_t  synth::decay_ms;      // decay period
extern uint16_t  synth::sustain;   // sustain volume
extern uint16_t  synth::release_ms;      // release period
extern uint16_t  synth::wave_vector;      // release period
extern uint16_t   synth::wave;

Adc adc;
Keys keys;


// ----------------------
//          LEDS
// ----------------------


void sr_shift_out(uint8_t val) {
  gpio_put(SR_LATCH, 0);
  for (int i = 0; i < SR_DATA_WIDTH; i++)  {
        gpio_put(SR_DATA, !!(val & (1 << i)));
        gpio_put(SR_CLK, 1);
        gpio_put(SR_CLK, 0);
  }
  gpio_put(SR_LATCH, 1);
}
void sr_bit_on (int pin) {
  leds |= (1 << (7 - pin));
  sr_shift_out(leds);
}
void sr_bit_toggle (int pin) {
  leds ^= (1 << (7 - pin));
  sr_shift_out(leds);
}
void sr_clear(void) {
  leds = 0;
  sr_shift_out(leds);
}
void sr_off(void) {
  sr_shift_out(0);
}
void sr_cycle(int delay, int dir) {
  leds = 0;
  sr_clear();
  sleep_ms(delay);

  if (dir == UP) {
    for (int i = 0; i < 7; i++) {
      leds = (1 << (7 - i));
      sr_shift_out(leds);
      sleep_ms(delay);
    }
  } else {
    for (int i = 0; i < 7; i++) {
      leds = (1 << (i+1));
      sr_shift_out(leds);
      sleep_ms(delay);
    }
  }
  sr_clear();
}
void sr_print_pins(void) {
  
  // printf(" ");

  for(int i = 0; i < SR_DATA_WIDTH; i++)
  {
      printf(" ");

      if((leds >> ((SR_DATA_WIDTH-1)-i)) & 1)
          printf("1");
      else
          printf("0");

  }
  printf("\n");
}
void sr_init (void) {
  gpio_init(SR_DATA);
  gpio_init(SR_CLK);
  gpio_init(SR_LATCH);

  gpio_set_dir(SR_DATA, GPIO_OUT);
  gpio_set_dir(SR_CLK, GPIO_OUT);
  gpio_set_dir(SR_LATCH, GPIO_OUT);

  sr_clear();
}

void pwm_pin_init (int pin) {
  gpio_set_function(pin, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(pin);
  
  pwm_set_gpio_level(pin, 0);
  pwm_set_enabled(slice_num, true);
}
void pwm_output_polarity (void) {
  uint slice_num0 = pwm_gpio_to_slice_num(LEDR_PIN);
  uint slice_num1 = pwm_gpio_to_slice_num(LEDB_PIN);
  pwm_set_output_polarity(slice_num0, true, true);
  pwm_set_output_polarity(slice_num1, true, false);
}

void led_toggle (int pin) {
  led_state[pin] = !led_state[pin]; // toggle the led state
  gpio_put(pin, led_state[pin]); // send to LED
}
void led_put (int pin, bool state) {
  gpio_put(pin, state);
}
void led_flash (int pin, int repeats, int delay) {
  for (int r = 0; r < repeats; r++) {
    led_toggle(pin);
    sleep_ms(delay);
    led_toggle(pin);
    sleep_ms(delay);
  }
}
void leds_init (void) {

  gpio_init(PICO_LED_PIN); // set LED pin
  gpio_set_dir(PICO_LED_PIN, GPIO_OUT); // set LED pin to out
  
  gpio_init(LED_LFO_PIN);
  gpio_set_dir(LED_LFO_PIN, GPIO_OUT);

  gpio_init(LED_ARP_PIN);
  gpio_set_dir(LED_ARP_PIN, GPIO_OUT);


  pwm_pin_init(LEDR_PIN);
  pwm_pin_init(LEDG_PIN);
  pwm_pin_init(LEDB_PIN);

  pwm_output_polarity ();

  sr_init();
}

void rgb_set_off(void) {
  rgb_state = 0;
  pwm_set_gpio_level(LEDR_PIN, 0);
  pwm_set_gpio_level(LEDG_PIN, 0);
  pwm_set_gpio_level(LEDB_PIN, 0);
}
void rgb_update(int r, int g, int b) {
  rgb_state = 1;
  pwm_set_gpio_level(LEDR_PIN, r);
  pwm_set_gpio_level(LEDG_PIN, g);
  pwm_set_gpio_level(LEDB_PIN, b);
}
void rgb_update_8bit(int r, int g, int b) {
  rgb_state = 1;
  pwm_set_gpio_level(LEDR_PIN, (r<<8));
  pwm_set_gpio_level(LEDG_PIN, (g<<8));
  pwm_set_gpio_level(LEDB_PIN, (b<<8));
}
void rgb_recall (void) {
  rgb_state = 1;
  rgb_update(rgb_colour[0],rgb_colour[1],rgb_colour[2]);
}
void rgb_set_on (void) {
  rgb_state = 1;
  pwm_set_gpio_level(LEDR_PIN, 65535);
  pwm_set_gpio_level(LEDG_PIN, 65535);
  pwm_set_gpio_level(LEDB_PIN, 65535);
}
void rgb_flash (int repeats, int delay){
  if (rgb_state) {
    for (int r = 0; r < repeats; r++) {
      rgb_set_off();
      sleep_ms(delay);
      rgb_recall();
      sleep_ms(delay);
    }
  } else {
    for (int r = 0; r < repeats; r++) {
      rgb_recall();
      sleep_ms(delay);
      rgb_set_off();
      sleep_ms(delay);
    }
  }

  
}
void rgb_cycle (int speed) {
  
  // reset array to blue incase wer'e on a different colour
  rgb_colour[0] = 65535;
  rgb_colour[1] = 0;
  rgb_colour[2] = 0;  

  // Choose the colours to increment and decrement.
  for (int decColour = 0; decColour < 3; decColour += 1) {
    volatile int incColour = decColour == 2 ? 0 : decColour + 1;

    // cross-fade the two colours.
    for(int i = 0; i < 65535; i += 1) {
      rgb_colour[decColour] -= 1;
      rgb_colour[incColour] += 1;
      
      rgb_update(rgb_colour[0],rgb_colour[1],rgb_colour[2]);
      sleep_us(speed);
    }
  }
}
void rgb_preset (int preset) {
  switch (preset) {
    case 0: //red
      rgb_update_8bit(255, 0, 0);
      break;
    case 1: // purple
      rgb_update_8bit(74, 0, 181);
      break;
    case 2: // pink
      rgb_update_8bit(255, 0, 35);
      break;
    case 3: // teal
      rgb_update_8bit(0, 115, 150);
      break;
    case 4: // blue
      rgb_update_8bit(10, 0, 255);
      break;
    case 5: // orange
      rgb_update_8bit(255, 39, 0);
      break;
    case 6: // green
      rgb_update_8bit(15, 255, 0);
      break;
    case 7: // tutti frutti
      rgb_update_8bit(255, 100, 90);
      break;
  } 
}
void rgb_init (void) {
  rgb_preset(0);
}

void leds_test (int repeats, int delay) {
  // rgb_flash(2, 200);
  sr_cycle(delay, UP);
  led_flash(LED_LFO_PIN, repeats, delay);
  led_flash(LED_ARP_PIN, repeats, delay);
  rgb_cycle(delay/4);
  led_flash(LED_ARP_PIN, repeats, delay);
  led_flash(LED_LFO_PIN, repeats, delay);
  sr_cycle(delay, DOWN);
}



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


	if ( (!((k>>PAGE_KEY) & 1)) &&  (((k_last>>PAGE_KEY) & 1)) ){
    // press PAGE/SHIFT key
    set_page_flag(true);
    // use the following for timed buttons
    // shift_start = to_ms_since_boot (get_absolute_time());
    if (KEYS_PRINT_OUT) printf("Key: Page\n");
	}



	if ( (!((k>>LFO_KEY) & 1)) &&  (((k_last>>LFO_KEY) & 1)) ){
    toggle_lfo_flag();
	}


  if ( (!((k>>ARP_KEY) & 1)) &&  (((k_last>>ARP_KEY) & 1)) ){
    toggle_arp_flag();
	} 


  if ( (!((k>>PRESET_KEY) & 1)) &&  (((k_last>>PRESET_KEY) & 1)) ){
    //press preset key
    preset_start = to_ms_since_boot (get_absolute_time());
    if (KEYS_PRINT_OUT) printf("Key: Preset ON\n");
	}

	if ( (((k>>PRESET_KEY) & 1)) &&  (!((k_last>>PRESET_KEY) & 1)) ){
    //release preset key
    preset_end = to_ms_since_boot (get_absolute_time());
    if (preset_end - preset_start < LONG_PRESS){
      //short press
      if (KEYS_PRINT_OUT) printf("Key: Preset OFF - Short\n");
      //raise preset flag;
      preset_flag = true;
      preset++;
      preset&=0x7;
      rgb_preset(preset);
      preset_flag = false;
  
    }
    if (preset_end - preset_start > LONG_PRESS){
      //long press
      if (KEYS_PRINT_OUT) printf("Key: Preset OFF - Long\n");
  
      //preset save
      //raise preset_save_flag:
      //      - save current settings in the pagination settings (all knob arrays)
      //      - flashed rgb led twice
    }
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
  page_values[ADSR][0]=10; //A
  page_values[ADSR][1]=20; // D
  page_values[ADSR][2]=1023; // S
  page_values[ADSR][3]=200; // R
  page_values[MOD][0]=20; //A
  page_values[MOD][1]=20; // D
  page_values[MOD][2]=20; // S
  page_values[MOD][3]=20; // R
}
// read knobs and digital switches and handle pagination
void pagination_update(){

  if(get_page_flag()){
    uint8_t temp_pages = 0;

    page_change = true;

    current_page++;

    // ARP on
    if (get_arp_flag()) {
      temp_pages = MAX_PAGES; // sets the page loop to be it's full length
    }
    // ARP off
    if (!get_arp_flag()) temp_pages = (MAX_PAGES-1);
    
    // count the pages
    if (current_page >= temp_pages) current_page = 0;
    
    set_page(current_page);
    set_page_flag(false);
  }

  // if page has changed then protect knobs
  if(page_change){
    page_change = false; // set the Page change back to false so it can be read again...

    // bitmask the knob leds off
    sr_shift_out((leds &= 0xF)); 

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
      sr_shift_out((leds |= 1 << (7 - i)));
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
  shift_flag != shift_flag;
}
bool get_shift_flag (void){
  return shift_flag;
}

void set_page (uint8_t value) {
  // using a switch here so that I can easily change the LEDs... find a better way?
  switch (value) {
    case 0:
      page = 0;
      sr_clear();
      break;
    case 1:
      page = 1;
      sr_bit_toggle(LED_PAGE1);
      break;
    case 2:
      page = 2;
      sr_bit_toggle(LED_PAGE1);
      sr_bit_toggle(LED_PAGE2);
      break;
    case 3:
      page = 3;
      sr_bit_toggle(LED_PAGE2);
      sr_bit_toggle(LED_PAGE3);
      break;
}
}
void set_page_flag(uint8_t value) {
  page_flag = value;
}
uint8_t get_page_flag(void) {
  return page_flag;
}

void set_lfo_flag(uint8_t value) {
  lfo_flag = value;
  led_put(LED_LFO_PIN, value);
}
void toggle_lfo_flag(void) {
  led_toggle(LED_LFO_PIN);
  lfo_flag = !lfo_flag;
  if (KEYS_PRINT_OUT) printf("Key: LFO\n");
}
uint8_t get_lfo_flag(void) {
  return lfo_flag;
}

void set_arp_flag(uint8_t value) {
  arp_flag = value;
  led_put(LED_ARP_PIN, value);
}
void toggle_arp_flag(void) {
  arp_flag = !arp_flag;
  led_toggle(LED_ARP_PIN);
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

  leds_init();
  rgb_init();
  keys.init();
  adc.init();
  pagination_init();
  

  puts("Welcome to the jungle...");

  if (HARDWARE_TEST) hardware_test(10);

  hardware_index = 0;
}

void hardware_test (int delay) {
  leds_test(1, delay);
}

void hardware_task (void) {
  if (hardware_index == 0) {
    keys.read();
  }
  if (hardware_index == 63) {
    keys_update();
  }
  if (hardware_index == 127) {
    // knobs_update();
    adc.update();
  }
  if (hardware_index == 191) {
    pagination_update();
    // synth::waveforms = 1<<(get_pagintaion(0,2)>>7);
    // if (shift_flag) synth::waveforms |= 1<<(get_pagintaion(0,2)>>7);
    // synth::waveforms = (16 * ((get_pagintaion(0,2)>>7)+1));      // bitmask for enabled waveforms (see AudioWaveform enum for values)
  }
  // --------------- //
  // PAGE 0 (GLOBAL) //
  // --------------- //
  
  // 1
  // ??? SHOULD BE MOD RATE ???
  synth::wave = ((get_pagintaion(0,0)>>6)*256);
  // 2
  // ??? SHOULD BE MOD DEPTH ???
  synth::wave_vector = (get_pagintaion(0,1)<<2);
  // 3

  // waveVec = pageValues[0][2];
  // synth::waveforms = map(get_pagintaion(0,2),0,1023,1,128);      // bitmask for enabled waveforms (see AudioWaveform enum for values)
  // synth::waveforms = 1<<(get_pagintaion(0,2)>>7);
  // 4
  pitch_scale = get_pitch_log(get_pagintaion(0,3)); // might need optional stability/lofi switch...
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
  // // }
  // ----------------------- //
  // PAGE 2 (LFO)            //
  // ----------------------- //
  // 1 - MATRIX
  modulation::set_matrix((uint8_t)(get_pagintaion(2,0)>>10));
  // 2 - RATE
  modulation::set_rate(get_pagintaion(2,1));
  // 3 - DEPTH
  modulation::set_depth(get_pagintaion(2,2));
  // 4 - WAVESHAPE
  modulation::set_wave(get_pagintaion(2,3)>>6);
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
  
  if (KNOBS_PRINT_OUT) {
    if (hardware_index==200) {
      print_knob_page();
      }
  }
  // hardware_index++;
  // hardware_index&=0xff;
}