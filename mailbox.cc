#include "mailbox.h"

namespace MAILBOX {

    note_data core0NoteData;
    note_data core1NoteData;

    note_data mail;

    void init() {
        mailbox_initialized = true;
    }

    void send() {
        if (!mailbox_initialized) {
            return;
        }
        if (!reading) {
            mail = core1NoteData;
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
            core0NoteData = mail;
            youvegotmail = false;
            reading = false;
        }
        else {
            return;
        }
    }
}







    // void update() {
    //     if (mutex_try_enter(&sharedDataMutex, NULL)) {
    //         core0NoteData = core1NoteData;
    //         unlock();
    //     } else {
    //         return;
    //     }
    // }