#include "modulation.h"


namespace MOD {


    // Modulation LFO(SAMPLE_RATE/8); // reduced speed poly LFO
    Modulation LFO(SAMPLE_RATE); // full speed mono LFO

    void Init () {
        LFO.set_matrix(0);
    }
    void Update () {
        LFO.Update();
    }
    void clear () {
        LFO.clear();
    }

    void set_state (bool input) {
        LFO.set_state(input);
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
        LFO.set_shape(input);
    }


}