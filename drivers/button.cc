#include "button.h"

namespace Buttons {
    
    void Button::pressed () {
        uint32_t time_now = to_ms_since_boot(get_absolute_time());
        if (time_now - _last_press_time <= DOUBLE_PRESS_TIME) {
            // double press detected
            _double = true;
        }
        _last_press_time = time_now;
        _shift = !_shift;
        _start = time_now;
    }

    void Button::released () {
        _shift = !_shift;
        _end = to_ms_since_boot(get_absolute_time());
        // short press
        if (_end - _start < LONG_PRESS_TIME) {
            _short = true;
        }
        // long press
        if (_end - _start > LONG_PRESS_TIME) { 
            _long = true;
        }
    }

  bool Button::get(ButtonState state) {
        switch(state) {
            case ButtonState::SHORT:
                if (_short) {
                    _short = false;
                    return true;
                }
                break;
            case ButtonState::LONG:
                if (_long) {
                    _long = false;
                    return true;
                }
                break;
            case ButtonState::DOUBLE:
                if (_double) {
                    _double = false;
                    return true;
                }
                break;
            case ButtonState::SHIFT:
                return _shift;
            default:
                break;
        }
        return false;
    }

    Button PAGE;
    Button LFO;
    Button ARP;
    Button PRESET;
}