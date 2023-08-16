#include "queue.h"

namespace QUEUE {
    queue_t trigger_queue;
    queue_t release_queue;
    
    void init() {
        queue_init(&trigger_queue, sizeof(trigger_msg_t), 16);
        queue_init(&release_queue, sizeof(uint8_t), 16);
    }
    
    void trigger_send (uint8_t slot, uint8_t note, bool gate) {
        trigger_msg_t temp;
        temp.slot = slot;
        temp.note = note;
        temp.gate = gate;
        queue_try_add(&trigger_queue, &temp);
    }
    void trigger_receive (uint8_t &slot, uint8_t &note, bool &gate) {
        trigger_msg_t temp;
        queue_try_remove(&trigger_queue, &temp);
        slot = temp.slot;
        note = temp.note;
        gate = temp.gate;
    }
    uint8_t trigger_check_queue () {
        return queue_get_level(&trigger_queue);
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
    uint8_t release_check_queue (){
        return queue_get_level_unsafe(&release_queue);
    }
}