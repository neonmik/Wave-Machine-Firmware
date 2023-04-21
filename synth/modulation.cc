#include "modulation.h"


namespace MOD {


    Modulation LFO;
    // Modulation MOD2;

    void init (uint16_t sample_rate) {
        
        LFO.init();
        // MOD2.init();
    }

    void update () {
        set_matrix(MOD_DATA.matrix);
        set_depth(MOD_DATA.depth);
        set_rate(MOD_DATA.rate);
        set_wave(MOD_DATA.shape);
        
        LFO.update();
        // MOD2.update();
    }

    void clear () {
        // LFO.clear();
        // // MOD2.clear();
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
    void set_wave (uint16_t input) {
        LFO.set_wave(input);
    }
    void set_state (bool input) {
        // if (LFO.get_state() != input) {
        //     LFO.set_state(input);
        // }
    }
}