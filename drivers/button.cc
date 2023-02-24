#include "button.h"

namespace Buttons {
    void Button::pressed () {
        _shift = !_shift;
        _start = to_ms_since_boot (get_absolute_time());
    }
    void Button::released () {
        _shift = !_shift;
        _end = to_ms_since_boot (get_absolute_time());
        // short press
        if (_end - _start < LONG_PRESS) {
            _short = true;
        }
        // long press
        if (_end - _start > LONG_PRESS) { 
            _long = true;
        }
        
    }
    bool Button::get_short () {
        bool temp = _short;
        _short = false;
        return temp;
    }
    bool Button::get_long () {
        bool temp = _long;
        _long = false;
        return temp;
    }
    bool Button::get_double () {
        bool temp = _double;
        _double = false;
        return temp;
    }
    bool Button::get_shift () {
        return _shift;
    }

    Button PAGE;
    Button LFO;
    Button ARP;
    Button PRESET;
}