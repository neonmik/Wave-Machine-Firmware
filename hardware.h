#pragma once

#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/spi.h"

#include "synth/synth.h"





#define abs(x) ({ __typeof__(x) _x = (x); _x >= 0 ? _x : -_x; })

#define KEYS_PRINT_OUT      0
#define KNOBS_PRINT_OUT     0
#define HARDWARE_TEST       0

#define MAX_PAGES         4 // the max number of pages available
#define MAX_KNOBS         4 // the max number of knobs available

#define PROTECTED        -1
#define ACTIVE            1

enum Page : uint8_t{
    MAIN    = 0,
    ADSR    = 1,
    LFO     = 2,
    ARP     = 3
};

#define protection_value  10 // the amount of protection the knob gets before unlocking.


// #define LED_KNOB1       0
// #define LED_KNOB2       1
// #define LED_KNOB3       2
// #define LED_KNOB4       3
// #define LED_PAGE1       4
// #define LED_PAGE2       5
// #define LED_PAGE3       6

namespace beep_machine {
    namespace {
        // preset, page and event flags
        uint8_t preset              =           0;
        uint8_t page                =           0;
        bool page_flag              =           0;
        bool lfo_flag               =           0;
        bool arp_flag               =           0;
        bool preset_flag            =           0;
        bool pagination_flag        =           0;
        bool shift_flag             =           0;

        // pagination
        uint32_t knobs[8];
        uint32_t page_values[MAX_PAGES][MAX_KNOBS]; // the permanent storage of every value for every page, used by the actual music code
        uint32_t knob_values[MAX_KNOBS]; // last read knob values
        uint8_t knob_states[MAX_KNOBS]; // knobs state (protected, enable...)

        uint32_t value           = 0; // current (temporary) value just read
        uint8_t current_page    = 0; // the current page id of values being edited
        bool page_change    = false; // signals the page change
        bool in_sync        = false; //temp variable to detect when the knob's value matches the stored value
    }
    
    


    
    
    uint32_t get_pagintaion (int page, int knob);
    uint8_t get_pagination_flag ();

    void toggle_shift_flag (void);
    bool get_shift_flag (void);

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
    
};


// ----------------------
//          KEYS
// ----------------------

void keys_update();

// ----------------------
//          KNOBS
// ----------------------

void pagination_init (void);
void default_pagination (void);
void pagination_update (void);

void print_knob_array (int *array, int len);
void print_knob_page (void);

// ----------------------
//        HARDWARE
// ----------------------

void hardware_init (void);
void hardware_test (int delay);
void hardware_task (void);
void hardware_debug (void);
