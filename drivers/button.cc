#include "button.h"

namespace Buttons {
    
    void Button::pressed () {
        uint32_t currentTime = to_ms_since_boot(get_absolute_time());
        if (currentTime - lastTime <= DOUBLE_PRESS_TIME) {
            // double press detected
            doublePressed = true;
        }
        lastTime = currentTime;
        shiftPressed = true;
        start = currentTime;
    }
    void Button::released () {
        // REMOVE THIS IF BOTH SHIFT AND LONG NEED TO HAPPEN
        if (!shiftPressed) return; // return because shift was turned off by long press
        
        shiftPressed = false;
        end = to_ms_since_boot(get_absolute_time());
        // short press
        if (end - start < LONG_PRESS_TIME) {
            shortPressed = true;
        }
        // long press
        if (end - start > LONG_PRESS_TIME) { 
            longPressed = true;
        }
    }
    bool Button::get(State state) {
        switch(state) {
            case State::SHORT:
                if (shortPressed) {
                    shortPressed = false;
                    return true;
                }
                break;
            case State::LONG:
                if (longPressed) {
                    longPressed = false;
                    return true;
                } 

                // REMOVE THIS IF BOTH SHIFT AND LONG NEED TO HAPPEN
                if (shiftPressed) {
                    uint32_t currentTime = to_ms_since_boot(get_absolute_time());
                    if (currentTime - start > LONG_PRESS_TIME && !longPressed) {
                        shiftPressed = false; // shutdown shift
                        return true;
                    }
                }
                break;
            case State::DOUBLE:
                if (doublePressed) {
                    doublePressed = false;
                    return true;
                }
                break;
            case State::SHIFT:
                return shiftPressed;
            default:
                break;
        }
        return false;
    }

    Button PAGE;
    Button FUNC1;
    Button FUNC2;
    Button PRESET;
}