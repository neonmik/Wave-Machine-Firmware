#pragma once

#include "pico/stdlib.h"

#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "pico/sync.h"


namespace MAILBOX {
    template <typename T>
        class Mailbox {
            public:
                Mailbox() : _locked(false), mailbox_initialized(false), reading(false), youvegotmail(false) {}
                T core0;
                T core1;
                T mail;

                void init() {
                    mailbox_initialized = true;
                }

                void send() {
                    if (!mailbox_initialized) {
                        return;
                    }
                    if (!reading) {
                        mail = core1;
                        youvegotmail = true;
                    }
                    else {
                        return;
                    }
                }

                void receive() {
                    if (!mailbox_initialized) {
                        return;
                    }
                    if (youvegotmail) {
                        reading = true;
                        core0 = mail;
                        youvegotmail = false;
                        reading = false;
                    }
                    else {
                        return;
                    }
                }

            private:
                bool _locked;
                bool mailbox_initialized;
                bool reading;
                bool youvegotmail;
        };

    struct note_data {
        uint8_t     note_state[128];
        uint8_t     notes_on        =       0;
    };

    struct trigger_msg_t {
        uint8_t     slot; // voice slot where note is now trigged
        uint8_t     note; // MIDI note number
    };

    extern queue_t trigger_queue;
    extern queue_t release_queue;
        
    extern Mailbox<note_data> NOTE_DATA;


    
    void init (void);
    void receive (void);
}
