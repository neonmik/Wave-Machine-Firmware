#include "keys.h"

void Keys::init(){
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

  gpio_init(DEBUG_PIN); // set LED pin
  gpio_set_dir(DEBUG_PIN, GPIO_OUT); // set LED pin to out

}
void Keys::read(){
  
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
    gpio_put(MUX_SEL_B, i & 2);
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    gpio_put(MUX_SEL_C, i & 4);
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    gpio_put(MUX_SEL_D, i & 8);
      
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    
    keys_history[history_index] |= gpio_get(MUX_OUT_0) << (i);
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    keys_history[history_index] |= gpio_get(MUX_OUT_1) << (i + 16);
    
    
  }
  // masks 32th key (spare pin in) and function keys - saves pulling floating pins high
  keys_history[history_index] |= 0x1; 

  // loops round history index
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
    keys = reverse(keys_history[0]);
	}

}
