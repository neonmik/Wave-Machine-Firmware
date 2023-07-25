#include "mailbox.h"

namespace MAILBOX {
    Mailbox<note_data> NOTE_DATA;
    
    void init() {
        NOTE_DATA.init();
    }
    void send() {
        NOTE_DATA.send();
    }
    void receive() {
        NOTE_DATA.receive();
    }
    void update() {

    }
}