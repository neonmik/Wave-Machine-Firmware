#pragma once

#include "pico/stdlib.h"

#include "pico/multicore.h"
#include "pico/util/queue.h"


namespace QUEUE {
    struct trigger_msg_t {
        uint8_t     slot;       // voice slot where note is now trigged
        uint8_t     note;       // MIDI note number
        bool        gate;

    };

    extern queue_t trigger_queue;
    extern queue_t release_queue;
        
    void init (void);

    void trigger_send (uint8_t slot, uint8_t note, bool gate);
    void trigger_receive (uint8_t &slot, uint8_t &note, bool &gate);
    uint8_t trigger_check_queue (void);
    
    void release_send (uint8_t slot);
    uint8_t release_receive (void);
    uint8_t release_check_queue (void);

}
