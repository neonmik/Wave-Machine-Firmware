#pragma once

#include "pico/stdlib.h"

#include "pico/multicore.h"
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

    struct voice_data {

    };
    
    extern Mailbox<note_data> NOTE_DATA;
    // extern Mailbox<synth_data> SYNTH_DATA;
    // extern Mailbox<mod_data> MOD_DATA;
    // extern Mailbox<arp_data> ARP_DATA;

    
    void init (void);
    // void send (uint8_t index);
    // void send_synth (void);
    // void send_mod (void);
    // void send_arp (void);
    void receive (void);
}
