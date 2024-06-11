#include "mux.h"

namespace MUX {

    void init (void) {
        pinInit(MUX_SEL_A);
        pinInit(MUX_SEL_B);
        pinInit(MUX_SEL_C);
        pinInit(MUX_SEL_D);

        transferAddress();
    }

    void incrementAddress (void) {
        address = (address + 1) % 16;
        
        transferAddress();
    }

    uint8_t getAddress (void) {
        return address;
    }
}