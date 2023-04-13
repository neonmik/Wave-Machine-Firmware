#pragma once

#include "pico/stdlib.h"

#include "pico/multicore.h"
#include "pico/sync.h"

namespace MAILBOX {
    
    
    // static mutex_t sharedDataMutex;
    
    static bool _locked = false;
    static bool mailbox_initialized = false;
    static bool reading = false;
    static bool youvegotmail = false;

    struct note_data {
        uint8_t     note_state[128];
        uint8_t     notes_on        =       0;
    };

    extern note_data core0NoteData;
    extern note_data core1NoteData;
    extern note_data mail;

    void init(void);
    void lock(void);
    void unlock(void);

    void send();
    void receive();

}
