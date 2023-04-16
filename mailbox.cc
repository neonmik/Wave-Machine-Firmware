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
    void send() {
        SYNTH_DATA.send();
        MOD_DATA.send();
        ARP_DATA.send();
    }
    void receive() {
        NOTE_DATA.receive();
        SYNTH_DATA.receive();
        MOD_DATA.receive();
        ARP_DATA.receive();
    }
}