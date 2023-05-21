#ifndef DAC_H_
#define DAC_H_

#include <math.h>

#include "pico/stdlib.h"

#include "hardware/irq.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

#include "../ui.h"

#include "../synth/beat_clock.h"

#define DAC_DATA        11
#define DAC_CLK         10
#define DAC_CS          9
#define DAC_SPI         spi1

#define DAC_CONFIG      0b0111000000000000


#define BUFFER_SIZE     64


#define size_bits       log2(BUFFER_SIZE * sizeof(uint16_t))

typedef uint16_t (*synth_function)();

extern uint32_t sample_clock;

extern uint32_t hardware_index; // eventually going to be used to free some resources from the DMA ISR
extern uint32_t software_index; // will be used to check currently timing of hardware_index


namespace DAC {
    

    namespace {

        synth_function process;

        bool        _full;
        
        uint32_t    _clock_speed;
        uint16_t    _sample_rate;
        uint16_t     _buffer_size    = BUFFER_SIZE;
        uint16_t    _buffer[BUFFER_SIZE];

        volatile uint16_t buf_a[BUFFER_SIZE] __attribute__((aligned(BUFFER_SIZE * sizeof(uint16_t))));
        volatile uint16_t buf_b[BUFFER_SIZE] __attribute__((aligned(BUFFER_SIZE * sizeof(uint16_t))));
        
        float       _divider;
        unsigned int slice_num;
        int dma_chan_a, dma_chan_b;

        void dac_send (uint16_t* value) {
            gpio_put(DAC_CS, 0);
            spi_write16_blocking(DAC_SPI, value, 1);
            gpio_put(DAC_CS, 1);
            ++sample_clock;
            ++hardware_index;
            hardware_index &= 0xf; //loop the buffer every 15 samples
        }


        void dma_buffer(uint16_t* buf) {
            for (int i = 0; i < _buffer_size; i++) {
                buf[i] = (process()) | (DAC_CONFIG);
                ++sample_clock;
                BEAT_CLOCK::tick();
            }
            _full = true;
        }
        void dma_channel (int dma_chan, int dma_chan_chain, volatile uint16_t* buf) {

            dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

            // 16 Bit transfers
            channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
            // increments read address after channel transfer
            channel_config_set_read_increment(&cfg, true);
            // sets the wrapping on the buffer (?)
            channel_config_set_ring(&cfg, false, size_bits);
            // set Timer 0 for timer pacing
            channel_config_set_dreq(&cfg, 0x3b);
            // selects next channel (in this case filp flops between channel A and B) 
            channel_config_set_chain_to(&cfg, dma_chan_chain);

            dma_channel_configure(
                dma_chan,             		// Channel to be configured
                &cfg,                 		// The configuration we just created
                &spi_get_hw(DAC_SPI)->dr, 	// write address
                buf,                  		// The initial read address
                _buffer_size,          		// Number of transfers
                false                 		// Start immediately?
            );
        }
        void dma_handler(void) {
            if(dma_hw->intr & (1u<<dma_chan_a)) { // channel a complete?
                dma_hw->ints0=1u<<dma_chan_a; // clear the interrupt request
                dma_buffer((uint16_t*) buf_a); // buf a transferred, so refill it
                // return; // trying this for a flip flop effect
            }
            if(dma_hw->intr & (1u<<dma_chan_b)) { // channel b complete?
                dma_hw->ints0=1u<<dma_chan_b; // clear the interrupt request
                dma_buffer((uint16_t*) buf_b); // buf b transferred, so refill it
                // return; // trying this for a flip flop effect
            }
        }
    
        void init_spi (uint16_t sample_rate) {
            spi_init(DAC_SPI, 20000000); 
            spi_set_format(DAC_SPI, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); // New SPI setup

            // set DAC pins
            gpio_set_function(DAC_DATA, GPIO_FUNC_SPI);
            gpio_set_function(DAC_CLK, GPIO_FUNC_SPI);
            gpio_set_function(DAC_CS, GPIO_FUNC_SPI);
        }

        void init_dma (void) {
            
            // Get a free channel, panic() if there are none
            dma_chan_a = dma_claim_unused_channel(true);
            dma_chan_b = dma_claim_unused_channel(true);

            // configures the two DMA's and links them too each other
            dma_channel(dma_chan_a, dma_chan_b, buf_a);
            dma_channel(dma_chan_b, dma_chan_a, buf_b);

            // sets up the interupt timer and points it to the handler
            irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
            dma_set_irq0_channel_mask_enabled((1<<dma_chan_a) | (1<<dma_chan_b), true);
            irq_set_enabled(DMA_IRQ_0,true);

            const int dma_timer = 0; // dma_claim_unused_timer(true); // panic upon failure
            dma_timer_claim(dma_timer); // panic if fail
            dma_timer_set_fraction(dma_timer, 1, _divider);

            dma_channel_start(dma_chan_a); // seems to start something
        }
        
    }

    void init (uint16_t sample_rate, synth_function audio_process);
    void clear_state (void);
    bool get_state (void);
}

#endif 