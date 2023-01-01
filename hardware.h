#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <stdio.h>

#include "hardware/adc.h"
// #include "hardware/irq.h" // don't think this is needed here
#include "hardware/pwm.h"
#include "hardware/spi.h"

#include "pagination.cpp"

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

#define SR_DATA_WIDTH   8
#define SR_DATA         18
#define SR_CLK          19
#define SR_LATCH        20

#define ADC_DIN         3
#define ADC_DOUT        4
#define ADC_CLK         2
#define ADC_CS          5

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


// ----------------------
//          LEDS
// ----------------------


void sr_shift_out(uint8_t val);
void sr_bit_on (int pin);
void sr_bit_toggle (int pin);
void sr_clear(void);
void sr_off(void);
void sr_cycle(int delay, int dir);
void sr_print_pins(void);
void sr_init (void);

void pwm_pin_init (int pin);
void pwm_output_polarity (void);

void led_toggle (int pin);
void led_put (int pin, bool state);
void led_flash (int pin, int repeats, int delay);
void leds_init (void);

void rgb_set_off(void);
void rgb_update(int r, int g, int b);
void rgb_update_8bit(int r, int g, int b);
void rgb_recall (void);
void rgb_set_on (void);
void rgb_flash (int repeats, int delay);
void rgb_cycle (int speed);
void rgb_preset (int preset);
void rgb_init (void);
void leds_test (int repeats, int delay);
void set_page (int page);


// ----------------------
//          KEYS
// ----------------------

void keys_init();
void keys_read();
void keys_update();

// ----------------------
//          KNOBS
// ----------------------

void knobs_init (void);
uint32_t knobs_read (uint8_t channel);
void knobs_update (void);



// ----------------------
//        HARDWARE
// ----------------------

void hardware_init (void);
void hardware_test (int delay);
void hardware_task (void);