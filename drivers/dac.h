#ifndef DAC_H_
#define DAC_H_

#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"

#include "hardware/irq.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

#define DAC_DATA        11
#define DAC_CLK         10
#define DAC_CS          9
#define DAC_SPI         spi1

#define BUF_SAMPLES     256

typedef int16_t (*buffer_callback)(void);


#define size_bits log2(BUF_SAMPLES * sizeof(uint16_t))  // 9, basically


static uint32_t clock_speed; // for storing MCU core frequency

static const uint16_t buffer_samples = 256; 
       

void dac_init (uint16_t sample_rate);
// void dac_buffer(int16_t sample);


static float sample_rate_div; //// for storing divider for DMA - usually 125m/44100 = 2'834.46712, but is dynamic to the input and the system clock.

static unsigned int slice_num;
static int dma_chan_a, dma_chan_b;

static volatile uint16_t buf_a[BUF_SAMPLES] __attribute__((aligned(BUF_SAMPLES * sizeof(uint16_t))));
static volatile uint16_t buf_b[BUF_SAMPLES] __attribute__((aligned(BUF_SAMPLES * sizeof(uint16_t))));

void dma_init (void);
void dma_channel (int dma_chan, int dma_chan_chain, volatile uint16_t* buf);
void dma_handler(void);
void dma_buffer(uint16_t* buf, uint pin);


#endif 