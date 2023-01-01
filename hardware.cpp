#define MUX_SEL_A       12
#define MUX_SEL_B       13
#define MUX_SEL_C       14
#define MUX_SEL_D       15
#define MUX_OUT_0       16
#define MUX_OUT_1       17

#define MAX_KEYS        27
#define PAGE_KEY        27
#define LFO_KEY         28
#define ARP_KEY         29
#define PRESET_KEY      30


uint32_t keys_history[] = {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
};
uint32_t keys      =    0xFFFFFFFF;
uint32_t keys_last =    0xFFFFFFFF;
uint8_t num_keys_down;

#define SR_DATA_WIDTH   8
#define SR_DATA         18
#define SR_CLK          19
#define SR_LATCH        20
uint8_t leds = 0xFF;

#define UP              1
#define DOWN            0

const int PICO_LED_PIN = PICO_DEFAULT_LED_PIN;

#define LED_LFO_PIN     21
#define LED_ARP_PIN     22
#define LEDR_PIN        6
#define LEDG_PIN        7
#define LEDB_PIN        8

#define LED_KNOB1       0
#define LED_KNOB2       1
#define LED_KNOB3       2
#define LED_KNOB4       3
#define LED_PAGE1       4
#define LED_PAGE2       5
#define LED_PAGE3       6

bool led_state[22];

bool rgb_state          = 0;
uint16_t rgb_colour[3]  = {65535, 0 ,0};


#define ADC_DIN         3
#define ADC_DOUT        4
#define ADC_CLK         2
#define ADC_CS          5

uint32_t software_index = 0;
uint32_t hardware_index = 2;

#define MAX_PRESETS     7

uint32_t knobs[8];

// public variable

int preset            = 0;
int page              = 0;
bool page_flag        = 0;
bool lfo_flag         = 0;
bool arp_flag         = 0;
bool preset_flag      = 0;

int preset_start;
int preset_end;

int long_press        = 400;



// functions



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
  rgb_cycle(delay/2);
  led_flash(LED_ARP_PIN, repeats, delay);
  led_flash(LED_LFO_PIN, repeats, delay);
  sr_cycle(delay, DOWN);
}

void set_page (int page) {
  switch (page) {
    case 0:
      sr_clear();
      break;
    case 1:
      sr_bit_toggle(LED_PAGE1);
      break;
    case 2:
      sr_bit_toggle(LED_PAGE1);
      sr_bit_toggle(LED_PAGE2);
      break;
    case 3:
      sr_bit_toggle(LED_PAGE2);
      sr_bit_toggle(LED_PAGE3);
      break;
  } 
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
		if ( !((k>>i) & 1) ) {
			// num_keys_down++;
		}
		if ( (!((k>>i) & 1)) &&  (((k_last>>i) & 1))  )  {  // new key down
      // 
      printf("Key: %d on\n", ((i) + 1));
			// note_priority(0x90, i+48, 127);   // keyboard starts at midi note 36
		}
		if ( ((k>>i) & 1) &&  (!((k_last>>i) & 1))  )  {  // key up
			// printf("Key: %d off\n", (i + 1));
      // note_priority(0x80, i+48 ,0);
      // set_note_off(i + 36);   // keyboard starts at midi note 36
			// dec_physical_notes_on();
		}
	}

	// check mode and aux button  (we only care about a mode press, but need press and release events for aux button)
	if ( (!((k>>PAGE_KEY) & 1)) &&  (((k_last>>PAGE_KEY) & 1)) ){
    page_flag = true;
    printf("Key: Page\n");
	}
	if ( (!((k>>LFO_KEY) & 1)) &&  (((k_last>>LFO_KEY) & 1)) ){
    led_toggle(LED_LFO_PIN);
    lfo_flag = !lfo_flag;
    printf("Key: LFO\n");
	}
  if ( (!((k>>ARP_KEY) & 1)) &&  (((k_last>>ARP_KEY) & 1)) ){
    led_toggle(LED_ARP_PIN);
    arp_flag = !arp_flag;
    printf("Key: ARP\n");
	}
  if ( (!((k>>PRESET_KEY) & 1)) &&  (((k_last>>PRESET_KEY) & 1)) ){
    //press preset key
    preset_start = to_ms_since_boot (get_absolute_time());
    printf("Key: Preset ON\n");
	}
	if ( (((k>>PRESET_KEY) & 1)) &&  (!((k_last>>PRESET_KEY) & 1)) ){
    //release preset key
    preset_end = to_ms_since_boot (get_absolute_time());
    if (preset_end - preset_start < long_press){
      //short press
      printf("Key: Preset OFF - Short\n");
      //raise preset flag;
      preset_flag = true;
      preset++;
      preset&=0x7;
      rgb_preset(preset);
      preset_flag = false;
  
    }
    if (preset_end - preset_start > long_press){
      //long press
      printf("Key: Preset OFF - Long\n");
  
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
  // new code
  adc_init();
  adc_gpio_init(26);
  adc_select_input(0);
  
  
  
  
  // old code
  // // initi SPI channel
  // spi_init(spi0, 4*1000*1000); // ** Try changing to 18Mhz, it should be able to take it
  
  // // set ADC pins
  // gpio_set_function(ADC_DOUT, GPIO_FUNC_SPI);
  // gpio_set_function(ADC_CLK, GPIO_FUNC_SPI);
  // gpio_set_function(ADC_DIN, GPIO_FUNC_SPI);
  
  // // set ADC chip select pin
  // gpio_init(ADC_CS);
  // gpio_set_dir(ADC_CS, GPIO_OUT);
  // gpio_put(ADC_CS, 1);

}
uint32_t knobs_read (uint8_t channel) {
  uint32_t sample = 0;
  uint32_t output = 0;

  gpio_put(MUX_SEL_A, channel & 1); 
  gpio_put(MUX_SEL_B, (channel >> 1) & 1);
  gpio_put(MUX_SEL_C, (channel >> 2) & 1);
  
  // nop needed to stop the processor reading too quickly, the mux cant keep up...
  asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
  
  // ----------
  // FIR filter
  // ----------
  // take 16 samples and add them together - slighty better

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
  
  //output result
  return output;

  // unsigned int commandout = 0;

  // // read a channel from the MCP3008 ADC
  // commandout = channel & 0x7; // only 0-7
  // commandout |= 0x18;        // start bit +single ended bit

  // uint8_t spibuf[3];

  // spibuf[0] = commandout;
  // spibuf[1] = 0;
  // spibuf[2] = 0;

  // gpio_put(ADC_CS, 0);  // Active low
  // spi_write_read_blocking(spi0, spibuf, spibuf, 3);
  // gpio_put(ADC_CS, 1);  // Active low

  // return ((spibuf[1] << 8) | (spibuf[2])) >> 4;
}
void knobs_update (void) {
  static uint8_t poll;

  // only need to read one every time as it's so quick

  knobs[poll] = knobs_read(poll);

  // old
  // knobs[poll] = knobs_read(poll);

  poll++;
  poll &= 0x3;
}

// ----------------------
//          DAC
// ----------------------

// void dac_init (void) {
//   // init SPI channel
//   spi_init(DAC_SPI, 20000000); 
//   spi_set_format(DAC_SPI, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); // New SPI setup

//   // set DAC pins
//   gpio_set_function(DAC_DATA, GPIO_FUNC_SPI);
//   gpio_set_function(DAC_CLK, GPIO_FUNC_SPI);
//   gpio_set_function(DAC_CS, GPIO_FUNC_SPI); // New CS setup - replaces next 3 lines
 
// }

// void fill_buffer(uint16_t* buf, uint pin)
// {
// 	// static volatile float x=0, y=0; // variables only visable in here, but important priority for memory?
// 	// const float pi2 = 6.27319; // pi squared...
// 	// const float dx = pi2 * wave_frequency/fs; // setup counter/index length pi2 * frequency of desired wave

//   uint16_t s = 0;
  
// 	// gpio_put(pin, 1); // pin output to test speed of loop
// 	for(int i =0; i<BUF_SAMPLES; i++) { // Number of samples loop = 256...
// 		// buf[i] = ((unsigned) cb()) | (0b0111<<12);
//     s = play_buf[i];
// 		buf[i] = (s) | (0b0111<<12); // buffer loads the associated sample value, and masks with the transfer infor for the DAC... Why is the buffer an array? 
//     play_buf[i] = 0;
//     // hardware_index++;
// 		// hardware_index &= 0xf;
// 		// x+= dx; // update the counter/index
// 		// if(x > pi2) x-= pi2; // check the counter/index for overflow

// 		// y = (sin(x)+1.0)*TOP/2.0; // calculate sample according to the index (sin(index)+1.0)* maxbits[4095]/2.0

// 	}
//   fill_buf = true;
// 	// gpio_put(pin, 0); // pin output to test speed of loop
// }
// void dma_handler()
// {
// 	if(dma_hw->intr & (1u<<dma_chan_a)) { // channel a complete?
// 		dma_hw->ints0=1u<<dma_chan_a; // clear the interrupt request
// 		fill_buffer((uint16_t*) buf_a, LED_LFO_PIN); // buf a transferred, so refill it
// 	}
// 	if(dma_hw->intr & (1u<<dma_chan_b)) { // channel b complete?
// 		dma_hw->ints0=1u<<dma_chan_b; // clear the interrupt request
// 		fill_buffer((uint16_t*) buf_b, LED_ARP_PIN); // buf b transferred, so refill it
// 	}

// }
// void dma_channel (int dma_chan, int dma_chan_chain, volatile uint16_t* buf) {
//   dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
	
//   channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
// 	channel_config_set_read_increment(&cfg, true);
// 	channel_config_set_ring(&cfg, false, size_bits);
// 	channel_config_set_dreq(&cfg, 0x3b); // timer pacing using Timer 0
// 	channel_config_set_chain_to(&cfg, dma_chan_chain); // start chan b when chan a completed

// 	dma_channel_configure(
// 			dma_chan,             // Channel to be configured
// 			&cfg,                 // The configuration we just created
// 			&spi_get_hw(DAC_SPI)->dr, // write address
// 			buf,                  // The initial read address
// 			BUF_SAMPLES,          // Number of transfers
// 			false                 // Start immediately?
// 			);
// }
// void dma_init (void) {
//   dma_chan_a = dma_claim_unused_channel(true);
// 	dma_chan_b = dma_claim_unused_channel(true);
// 	dma_channel(dma_chan_a, dma_chan_b, buf_a);
// 	dma_channel(dma_chan_b, dma_chan_a, buf_b);
// 	irq_set_exclusive_handler(DMA_IRQ_0,dma_handler);
// 	dma_set_irq0_channel_mask_enabled((1<<dma_chan_a) | (1<<dma_chan_b), true);
// 	irq_set_enabled(DMA_IRQ_0,true);

// 	const int dma_timer = 0; // dma_claim_unused_timer(true); // panic upon failure
// 	dma_timer_claim(dma_timer); // panic if fail
// 	dma_timer_set_fraction(dma_timer, 1, sample_rate_div);

//   dma_channel_start(dma_chan_a); // seems to start something
// }





// moveeeee thissss - just cause less errors at the minute due to my messy files, and inability to modularize

#include "pagination.cpp"



// ----------------------
//        HARDWARE
// ----------------------


void hardware_init (void) {
  leds_init();
  rgb_init();
  keys_init();
  knobs_init(); // need to write code to replace this for SR in
  // dac_init();

  pagination_init();

  stdio_init_all();

  puts("Welcome to the jungle...");
}

void hardware_test (int delay) {
  leds_test(1, delay);
}

void hardware_task (void) {
  keys_read();
  keys_update();
  knobs_update();
  pagination_get();
  
}