#include "modulation.h"


namespace MOD {


    Modulation LFO;

    void init (uint16_t sample_rate) {
        
        LFO.init();
    }
    void update () {

        LFO.update();
    }
    void clear () {
        // LFO.clear();
    }

    void set_state (bool input) {
        if (LFO.get_state() != input) {
            LFO.set_state(input);
        }
    }
    
    void set_matrix (uint16_t input) {
        LFO.set_matrix(input);
    }
    void set_depth (uint16_t input) {
        LFO.set_depth(input);
    }
    void set_rate (uint16_t input) {
        LFO.set_rate(input);
    }
    void set_shape (uint16_t input) {
        LFO.set_wave(input);
    }
    
}