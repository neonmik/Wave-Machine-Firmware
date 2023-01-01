#pragma once

#ifndef DAC_H_
#define DAC_H_

#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/irq.h"
#include "hardware/spi.h"
#include "hardware/dma.h"

#include <math.h>

// ---------------------------
//         DAC SETUP
// ---------------------------

#define DAC_DATA        11
#define DAC_CLK         10
#define DAC_CS          9
#define DAC_SPI         spi1

#define BUF_SAMPLES     256

typedef int16_t (*buffer_callback)(void);

const unsigned long clk_hz = 125000000; // MCU core frequency
const uint16_t audio_sample_rate = 44100; //selecting the sample rate (44100) - a ternary operator, basically setting result = 44100, if it was sample_rate = 0 ? 44100 : 22000, it would be 22000
const float sample_rate_div = clk_hz/ audio_sample_rate; //125m/44100 = 2'834.46712
const uint16_t fs = clk_hz/sample_rate_div; // doesnt make sense, you could just have this as the sample rate?
static uint16_t wave_frequency = 500; // wave frequency
static uint16_t wave_frequency2 = 100; // wave frequency
const uint16_t TOP = 4095; // highest possible sample number
static_assert(TOP <= 0xffff); // it's only a 16-bit register

static unsigned int slice_num;
static int dma_chan_a, dma_chan_b;

static volatile uint16_t buf_a[BUF_SAMPLES] __attribute__((aligned(BUF_SAMPLES * sizeof(uint16_t))));
static volatile uint16_t buf_b[BUF_SAMPLES] __attribute__((aligned(BUF_SAMPLES * sizeof(uint16_t))));



#define size_bits log2(BUF_SAMPLES * sizeof(uint16_t))  // 9, basically

// static bool fill_buf = 0;

void dac_init (void);
void dma_init (void);
void dma_channel (int dma_chan, int dma_chan_chain, volatile uint16_t* buf);
void dma_handler(void);
void fill_buffer(uint16_t* buf, uint pin);

#endif 






