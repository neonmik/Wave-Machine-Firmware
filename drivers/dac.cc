// ----------------------
//          DAC
// ----------------------

#include "dac.h"

uint16_t play_buffer[buffer_samples];
bool buffer_flag;

static uint32_t sample_clock; 

void dac_init (uint16_t sample_rate) {
	// init SPI channel
	spi_init(DAC_SPI, 18000000); 
	spi_set_format(DAC_SPI, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); // New SPI setup

	// set DAC pins
	gpio_set_function(DAC_DATA, GPIO_FUNC_SPI);
	gpio_set_function(DAC_CLK, GPIO_FUNC_SPI);
	gpio_set_function(DAC_CS, GPIO_FUNC_SPI); // New CS setup - replaces next 3 lines
	
	// dynamically set up sample rate for DMA
	clock_speed = clock_get_hz(clk_sys);
	sample_rate_div = clock_speed / sample_rate;

	play_buffer[buffer_samples];

	dma_init();
}

void dma_init (void) {
	dma_chan_a = dma_claim_unused_channel(true);
	dma_chan_b = dma_claim_unused_channel(true);
	dma_channel(dma_chan_a, dma_chan_b, buf_a);
	dma_channel(dma_chan_b, dma_chan_a, buf_b);
	irq_set_exclusive_handler(DMA_IRQ_0,dma_handler);
	dma_set_irq0_channel_mask_enabled((1<<dma_chan_a) | (1<<dma_chan_b), true);
	irq_set_enabled(DMA_IRQ_0,true);

	const int dma_timer = 0; // dma_claim_unused_timer(true); // panic upon failure
	dma_timer_claim(dma_timer); // panic if fail
	dma_timer_set_fraction(dma_timer, 1, sample_rate_div);

	dma_channel_start(dma_chan_a); // seems to start something
}
void dma_channel (int dma_chan, int dma_chan_chain, volatile uint16_t* buf) {
	dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
		
	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
	channel_config_set_read_increment(&cfg, true);
	channel_config_set_ring(&cfg, false, size_bits);
	channel_config_set_dreq(&cfg, 0x3b); // timer pacing using Timer 0
	channel_config_set_chain_to(&cfg, dma_chan_chain); // start chan b when chan a completed

	dma_channel_configure(
			dma_chan,             		// Channel to be configured
			&cfg,                 		// The configuration we just created
			&spi_get_hw(DAC_SPI)->dr, 	// write address
			buf,                  		// The initial read address
			BUF_SAMPLES,          		// Number of transfers
			false                 		// Start immediately?
			);
}

void dma_handler()
{
	if(dma_hw->intr & (1u<<dma_chan_a)) { // channel a complete?
		dma_hw->ints0=1u<<dma_chan_a; // clear the interrupt request
		dma_buffer((uint16_t*) buf_a, PICO_DEFAULT_LED_PIN); // buf a transferred, so refill it
	}
	if(dma_hw->intr & (1u<<dma_chan_b)) { // channel b complete?
		dma_hw->ints0=1u<<dma_chan_b; // clear the interrupt request
		dma_buffer((uint16_t*) buf_b, PICO_DEFAULT_LED_PIN); // buf b transferred, so refill it
	}

}

void dma_buffer(uint16_t* output_buffer, uint pin)
{
	static uint16_t sample = 0;
  
	// gpio_put(pin, 1); // pin output to test speed of loop
	for(int i =0; i<BUF_SAMPLES; i++) { // Number of samples loop = 256...
		sample = (play_buffer[i]);
		output_buffer[i] = 0;
		output_buffer[i] = (sample) | (0b0111<<12); // buffer loads the associated sample value, and masks with the transfer infor for the DAC...


	}
	// sample_clock++;
	// sample_clock&=0xff;
  	buffer_flag = true;
	// gpio_put(pin, 0); // pin output to test speed of loop
}




