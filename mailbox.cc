#include "mailbox.h"

namespace MAILBOX {
    Mailbox<note_data> NOTE_DATA;
    Mailbox<synth_data> SYNTH_DATA;
    Mailbox<mod_data> MOD_DATA;
    Mailbox<arp_data> ARP_DATA;
    
    void init() {
        NOTE_DATA.init();
        SYNTH_DATA.init();
        MOD_DATA.init();
        ARP_DATA.init();
    }
    void send(uint8_t index) {
        switch (index) {
            case 0:
                SYNTH_DATA.send();
                break;
            case 1:
                MOD_DATA.send();
                break;
            case 2:
                ARP_DATA.send();
                break;
        }
    }
    void send_synth() {
        SYNTH_DATA.send();
    }
    void send_mod() {
        MOD_DATA.send();
    }
    void send_arp() {
        ARP_DATA.send();
    }
    void receive() {
        static uint8_t index;
        switch (index) {
            case 0:
                SYNTH_DATA.receive();
                ++index;
                break;
            case 1:
                MOD_DATA.receive();
                ++index;
                break;
            case 2:
                ARP_DATA.receive();
                index = 0;
                break;
        }
        NOTE_DATA.receive();
    }
}