#include "adc.h"

void Adc::init() {
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

    // adc pin setup
    adc_gpio_init(MUX_OUT_ADC);
    adc_init();
    adc_select_input(0);

    mux_address_ = 0;
}

void Adc::update() {
    gpio_put(23, 1); // sets SMPS into low power mode for better reading on the ADC - need to validate... 

    // sets mux pins
    gpio_put(MUX_SEL_A, mux_address_ & 1); 
    gpio_put(MUX_SEL_B, (mux_address_ >> 1) & 1);
    gpio_put(MUX_SEL_C, (mux_address_ >> 2) & 1);
    gpio_put(MUX_SEL_D, (mux_address_ >> 3) & 1);

    // wait to read - allows settling time
    asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
    
    // IIR filter
    // passes last actual sample to a buffer
    // sample_[mux_address_] = values_[mux_address_];
    // filters new sample with old sample
    sample_[mux_address_] = sample_[mux_address_] - (sample_[mux_address_]>>4) + adc_read();

    // filters out lowest results, but might not be needed
    // output = (sample>>2);
    // if (output<=5) output = 0;

    // moves filtered sample to the adc array
    values_[mux_address_] = (sample_[mux_address_]>>4);
    

    // sets the index to loop
    mux_address_ = (mux_address_ + 1) % MAX_KNOBS;

    // puts SMPS back into PWM mode
    gpio_put(23, 0); 

    // zeros mux for keys
    gpio_put(MUX_SEL_A, 0);
    gpio_put(MUX_SEL_B, 0);
    gpio_put(MUX_SEL_C, 0);
    gpio_put(MUX_SEL_D, 0);
}