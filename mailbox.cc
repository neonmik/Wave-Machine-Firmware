#include "mailbox.h"

namespace MAILBOX {
    Mailbox<note_data> NOTE_DATA;

    queue_t trigger_queue;
    queue_t release_queue;
    
    void init() {
        NOTE_DATA.init();
        queue_init(&trigger_queue, sizeof(trigger_msg_t), 8);
        queue_init(&release_queue, sizeof(uint8_t), 8);
    }
    void send() {
        NOTE_DATA.send();
    }
    
    void trigger_send (uint8_t slot, uint8_t note) {
        trigger_msg_t temp;
        temp.slot = slot;
        temp.note = note;
        queue_try_add(&trigger_queue, &temp);
    }
    void trigger_receive (uint8_t &slot, uint8_t &note) {
        trigger_msg_t temp;
        queue_try_remove(&trigger_queue, &temp);
        slot = temp.slot;
        note = temp.note;
    }


    void release_send (uint8_t slot) {
        uint8_t _slot = slot;
        queue_try_add(&release_queue, &_slot);
    }
    uint8_t release_receive (void) {
        uint8_t temp;
        queue_try_remove(&release_queue, &temp);
        return temp;
    }

    void receive() {
        NOTE_DATA.receive();
    }
    void update() {

    }
}