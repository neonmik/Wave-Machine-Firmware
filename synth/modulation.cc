#include "modulation.h"


namespace MOD {


    // Modulation LFO(SAMPLE_RATE/8); // reduced speed poly LFO
    Modulation LFO(SAMPLE_RATE); // full speed mono LFO

    void init () {
        LFO.setMatrix(0);
    }
    void update () {
        LFO.update();
    }
    void clear () {
        LFO.clear();
    }

    void setState (bool input) {
        LFO.setState(input);
    }
    
    void setMatrix (uint16_t input) {
        LFO.setMatrix(input);
    }
    void setDepth (uint16_t input) {
        LFO.setDepth(input);
    }
    void setRate (uint16_t input) {
        LFO.setRate(input);
    }
    void setShape (uint16_t input) {
        LFO.setShape(input);
    }


}