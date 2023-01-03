#include "system.h"



uint32_t software_index = 0;
uint32_t hardware_index = 2;

#define MAX_PRESETS     7

using namespace beep_machine;

extern void note_priority(int status, int note, int velocity);


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

void keys_init(){
  gpio_init(MUX_SEL_A);
  gpio_init(MUX_SEL_B);
  gpio_init(MUX_SEL_C);
  gpio_init(MUX_SEL_D);

  gpio_set_dir(MUX_SEL_A, GPIO_OUT);
  gpio_set_dir(MUX_SEL_B, GPIO_OUT);
  gpio_set_dir(MUX_SEL_C, GPIO_OUT);
  gpio_set_dir(MUX_SEL_D, GPIO_OUT);

  gpio_set_dir(MUX_OUT_0, GPIO_IN);
  gpio_pull_up(MUX_OUT_0);
  gpio_set_dir(MUX_OUT_1, GPIO_IN);
  gpio_pull_up(MUX_OUT_1);

  gpio_put(MUX_SEL_A, 1);
  gpio_put(MUX_SEL_B, 1);
  gpio_put(MUX_SEL_C, 1);
  gpio_put(MUX_SEL_D, 1);

}
void keys_read(){
  //static, so it remains through iterations
  static uint8_t history_index;
  
  keys_history[history_index] = 0;
  
  gpio_put(MUX_SEL_A, 0);
  gpio_put(MUX_SEL_B, 0);
  gpio_put(MUX_SEL_C, 0);
  gpio_put(MUX_SEL_D, 0); 

  for (int i = 0; i < 16; i++) {
    // nop needed to stop the processor reading too quickly, the mux cant keep up...

    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    gpio_put(MUX_SEL_A, i & 1); 
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    gpio_put(MUX_SEL_B, (i >> 1) & 1);
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    gpio_put(MUX_SEL_C, (i >> 2) & 1);
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    gpio_put(MUX_SEL_D, (i >> 3) & 1);
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
      
    
    keys_history[history_index] |= gpio_get(MUX_OUT_0) << (i);
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    keys_history[history_index] |= gpio_get(MUX_OUT_1) << (i + 16);
    
    
  }
  
  keys_history[history_index] |= 0x1; // masks 32th key (spare pin in) and function keys - saves pulling floating pins high
  
  history_index++;
  history_index &= 0x7;
  
  if ((keys_history[0] == keys_history[1]) &&
		(keys_history[0] == keys_history[2]) &&
		(keys_history[0] == keys_history[3]) &&
		(keys_history[0] == keys_history[4]) &&
		(keys_history[0] == keys_history[5]) &&
		(keys_history[0] == keys_history[6]) &&
		(keys_history[0] == keys_history[7]))
	{
		// reverse bit order  - for messed up hardware
    unsigned int input = keys_history[0];
    unsigned int output = 0;
    
    for (int i = 0; i < 32; i++) {
      if ((input & (1 << i))) 
            output |= 1 << ((32 - 1) - i);
    }
    
    keys = output;
    // keys = keys_history[0];
	}

}
void keys_update() {
  uint32_t k, k_last;

	k =  keys;
	k_last = keys_last;
	num_keys_down = 0;

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

	// check mode and aux button  (we only care about a mode press, but need press and release events for aux button)
	if ( (!((k>>PAGE_KEY) & 1)) &&  (((k_last>>PAGE_KEY) & 1)) ){
    set_page_flag(true);
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
	keys_last = keys;
}

// ----------------------
//          KNOBS
// ----------------------

void knobs_init (void) {
  adc_init();
  adc_gpio_init(26);
  adc_select_input(0);
}
uint32_t knobs_read (uint8_t channel) {
  uint32_t sample = 0;
  uint32_t output = 0;

  // Translates channel number into a binary address for the mux
  gpio_put(MUX_SEL_A, channel & 1); 
  gpio_put(MUX_SEL_B, (channel >> 1) & 1);
  gpio_put(MUX_SEL_C, (channel >> 2) & 1);
  gpio_put(MUX_SEL_D, (channel >> 3) & 1);
  
  // nop needed to stop the processor reading too quickly, the mux cant keep up...
  asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
  
  // ----------
  // FIR filter
  // ----------
  // take 16 samples and add them together - slighty better but takes more time

  // for (int i = 0; i < 8; i++) {
  //   sample += adc_read();
  // }
  // output = ((sample/8)>>2); // divide samples to get average

  // ----------
  // IIR filter
  // ----------
  // take 1 sample, weighted against the previous ones
  sample = sample - (sample>>2) + adc_read();
  output = (sample>>2);
  if (output<=5) output = 0;
  
  gpio_put(MUX_SEL_A, 0);
  gpio_put(MUX_SEL_B, 0);
  gpio_put(MUX_SEL_C, 0);
  gpio_put(MUX_SEL_D, 0);
  //output result
  return output;
}
void knobs_update (void) {
  static uint8_t poll;

  gpio_put(23, 1); // sets SMPS into low power mode for better reading on the ADC - need to validate... 

  // only need to read one every time as it's so quick
  set_knob(poll, knobs_read(poll));
  
  gpio_put(23, 0); // puts SMPS back into PWM mode

  poll++;
  poll &= 0x3;
}

void set_knob(uint8_t knob, uint32_t value) {
  knobs[knob] = value;
}
uint32_t get_knob(uint8_t knob) {
  return knobs[knob];
}

void pagination_init() {
  // pinMode(aLED, OUTPUT);
  for(int i=0; i < MAX_KNOBS; i++){
    knob_values[i] = get_knob(i);
    knob_states[i] = ACTIVE;
  }
  default_pagination();
}
void default_pagination () {
  // ADSR default values
  page_values[ADSR][0]=0; //A
  page_values[ADSR][1]=0; // D
  page_values[ADSR][2]=1023; // S
  page_values[ADSR][3]=0; // R
}
// read knobs and digital switches and handle pagination
void pagination_get(){

  if(get_page_flag()){
    page_change = true;

    // current_page = !current_page;
    // Or 
    uint8_t temp_pages = 0;

    // increment the current page
    current_page++;

    // check to see if the arp is on, if so allow the page to open
    if (get_arp_flag()) {
      temp_pages = MAX_PAGES; // sets the page loop to be it's full length
    }
    if (!get_arp_flag()) temp_pages = (MAX_PAGES-1); // shrinks the loop so we dont end up with loads of pages we don't need when not using Arp.
    
    // keep the amount of pages right
    if (current_page >= temp_pages) current_page = 0;
    
    // set the page we're now on
    set_page(current_page);
    
    // clear the page flag for next time
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
    value = get_knob((uint8_t) i);
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
  keys_init();
  knobs_init(); // need to write code to replace this for SR in
  // dac_init();
  pagination_init();
  

  puts("Welcome to the jungle...");

  if (HARDWARE_TEST) hardware_test(10);

  hardware_index = 0;
}

void hardware_test (int delay) {
  leds_test(1, delay);
}

void hardware_task (void) {
  keys_read();
  keys_update();
  knobs_update();
  pagination_get();
  
  if (KNOBS_PRINT_OUT) {
    if (hardware_index==0) {
      print_knob_page();
      }
  }
  hardware_index++;
  hardware_index&=0xFF;
}