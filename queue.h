#pragma once


#include "config.h"

#include "pico/multicore.h"
#include "pico/util/queue.h"





namespace QUEUE {
    struct trigger_msg_t {
        uint8_t     slot;       // voice slot where note is now trigged
        uint8_t     note;       // MIDI note number
        uint8_t     velocity;   // MIDI velocity
        bool        gate;

    };

    extern queue_t trigger_queue;
    extern queue_t release_queue;
        
    void init (void);
    void update (void);

    void triggerSend (uint8_t slot, uint8_t note, bool gate);
    bool triggerReceive (uint8_t &slot, uint8_t &note, bool &gate);
    bool triggerCheckQueue (void);
    
    void releaseSend (uint8_t slot);
    uint8_t releaseReceive (void);
    uint8_t releaseCheckQueue (void);

}
