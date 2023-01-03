#pragma once

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/spi.h"

#define abs(x) ({ __typeof__(x) _x = (x); _x >= 0 ? _x : -_x; })

#define KEYS_PRINT_OUT      0
#define KNOBS_PRINT_OUT     0
#define HARDWARE_TEST       0

#define UP              1
#define DOWN            0

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

#define LONG_PRESS      400

#define SR_DATA_WIDTH   8
#define SR_DATA         18
#define SR_CLK          19
#define SR_LATCH        20

#define ADC_DIN         3
#define ADC_DOUT        4
#define ADC_CLK         2
#define ADC_CS          5

#define MAX_PAGES         4 // the max number of pages available
#define MAX_KNOBS         4 // the max number of knobs available

#define PROTECTED        -1
#define ACTIVE            1

#define MAIN              0
#define ADSR              1
#define MOD               2
#define ARP               3

#define protection_value  5 // the amount of protection the knob gets before unlocking.

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

namespace beep_machine {
    // keys 
    static uint32_t keys_history[8]    =           {
                                                0xFFFFFFFF,
                                                0xFFFFFFFF,
                                                0xFFFFFFFF,
                                                0xFFFFFFFF,
                                                0xFFFFFFFF,
                                                0xFFFFFFFF,
                                                0xFFFFFFFF,
                                                0xFFFFFFFF,
                                            };
    static uint32_t keys               =           0xFFFFFFFF;
    static uint32_t keys_last          =           0xFFFFFFFF;
    static uint8_t num_keys_down       =           0;

    // preset, page and event flags
    static uint8_t preset              =           0;
    static uint8_t page                =           0;
    static bool page_flag              =           0;
    static bool lfo_flag               =           0;
    static bool arp_flag               =           0;
    static bool preset_flag            =           0;

    static uint32_t preset_start;
    static uint32_t preset_end;

    // knobs
    static uint32_t knobs[8];
    static uint32_t page_values[MAX_PAGES][MAX_KNOBS]; // the permanent storage of every value for every page, used by the actual music code
    static uint32_t knob_values[MAX_KNOBS]; // last read knob values
    static uint8_t knob_states[MAX_KNOBS]; // knobs state (protected, enable...)

    static uint32_t value           = 0; // current (temporary) value just read
    static uint8_t current_page    = 0; // the current page id of values being edited
    static bool page_change    = false; // signals the page change
    static bool in_sync        = false; //temp variable to detect when the knob's value matches the stored value

    // leds
    static uint8_t leds                =           0xFF;
    static bool led_state[22];

    static bool rgb_state              =           0;
    static uint16_t rgb_colour[3]      =           {65535, 0 ,0};

    
};


void set_page (uint8_t page);
void set_page_flag(uint8_t value);
uint8_t get_page_flag(void);

void set_lfo_flag(uint8_t value);
void toggle_lfo_flag(void);
uint8_t get_lfo_flag(void);

void set_arp_flag(uint8_t value);
void toggle_arp_flag(void);
uint8_t get_arp_flag(void);

void set_preset(uint8_t preset);
void change_preset(void);
uint8_t get_preset(void);
void set_preset_flag(uint8_t value);
uint8_t get_preset_flag(void);

void set_knob(uint8_t knob, uint32_t value);
uint32_t get_knob(uint8_t knob);


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

void pagination_init (void);
void default_pagination (void);
void pagination_get (void);
void print_knob_array (int *array, int len);
void print_knob_page (void);

// ----------------------
//        HARDWARE
// ----------------------

void hardware_init (void);
void hardware_test (int delay);
void hardware_task (void);

#endif