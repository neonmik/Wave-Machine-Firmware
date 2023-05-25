#pragma once

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

#define DEBUG_PIN       2

#define LONG_PRESS      400

#include "pico/stdlib.h"

#include "button.h"

#include "../mailbox.h"

#include "../midi.h"

namespace KEYS {

    class Keyboard {
        private:
            uint32_t _history[8]    =       
            {
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
                0xFFFFFFFF,
            };

            uint32_t    _current         =       0xFFFFFFFF;
            uint32_t    _last            =       0xFFFFFFFF;
            
            uint8_t     history_index;

            
            

            uint32_t reverse(uint32_t input) {
                uint32_t output = 0;
                for (int i = 0; i < 32; i++) {
                    if ((input & (1 << i))) output |= 1 << ((32 - 1) - i);
                }
                return output;
            }
        public:
            
            Keyboard() { }
            ~Keyboard() { }

            void init();
            void read();
            inline int32_t get() const {
                return _current;
            }
            inline int32_t get_last() const {
                return _last;
            }
            inline void set_last(uint32_t last) {
                _last = last;
            }

    };
    namespace {
        MAILBOX::note_data& NOTES = MAILBOX::NOTE_DATA.core1;
        
        bool       _changed         =       false;

        void inc_physical_notes() {
            NOTES.notes_on++;
        }
        void dec_physical_notes() {
            if (NOTES.notes_on) NOTES.notes_on--; //checks to make sure there is a note on
        }
    }
    
    extern Keyboard Keys;

    void init (void);
    void read (void);
    void update (void);

    void note_on (uint8_t note);
    void note_off (uint8_t note);
    void note_clear (void);
    uint8_t get_notes_on (void);
}
