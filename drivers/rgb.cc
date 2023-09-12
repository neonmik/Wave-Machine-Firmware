#include "rgb.h"

namespace RGB_LED {
    //private namespace, only available in rgb.cc
    namespace {
        uint16_t _buffer[3];
    }

    rgb Rgb(LEDR_PIN, LEDG_PIN, LEDB_PIN);
    
    void Init () {
        Rgb.Init();
    }
    void on () {
        Rgb.set_buffer(_buffer[0], _buffer[1], _buffer[2]);
    }
    void Update () {
        Rgb.Update();
    }
    void set (uint16_t red, uint16_t green, uint16_t blue) {
        _buffer[0] = red;
        _buffer[1] = green;
        _buffer[2] = blue;
        Rgb.set_buffer(_buffer[0], _buffer[1], _buffer[2]);
    }
    void off () {
        Rgb.set_buffer(0, 0, 0);
    }
    void clear () {
        _buffer[0] = 0;
        _buffer[1] = 0;
        _buffer[2] = 0;
        Rgb.clear_buffer();
    }
}


