#include "button.h"

namespace Buttons {
    
    void Button::pressed () {
        uint32_t currentTime = to_ms_since_boot(get_absolute_time());
        if (currentTime - lastTime <= DOUBLE_PRESS_TIME) {
            // double press detected
            doublePress = true;
        }
        lastTime = currentTime;
        shiftPress = true;
        start = currentTime;
    }
    void Button::released () {
        
        shiftPress = false;

        uint32_t length = to_ms_since_boot(get_absolute_time()) - start;
        
        // short press
        if ((length > DEBOUNCE_TIME) && (length < SHORT_PRESS_TIME)) {
            shortPress = true;
        }

        // long press
        if ((length > LONG_PRESS_TIME)) {
            longPress = true;
        }

        // Call short press action if set
        if (shortPress && shortPressAction) {
            shortPressAction();
            shortPress = false;
        }

        // Call long press action if set
        if (longPress && longPressAction) {
            longPressAction();
            longPress = false;
        }
    }
    bool Button::get(State state) {
        switch(state) {
            case State::SHORT:
                if (shortPress) {
                    shortPress = false;
                    return true;
                }
                break;
            case State::LONG:
                if (longPress) {
                    longPress = false;
                    return true;
                } 

                // REMOVE THIS IF BOTH SHIFT AND LONG NEED TO HAPPEN
                if (shiftPress) {
                    uint32_t currentTime = to_ms_since_boot(get_absolute_time());
                    if (currentTime - start > LONG_PRESS_TIME && !longPress) {
                        shiftPress = false; // shutdown shift
                        return true;
                    }
                }
                break;
            case State::DOUBLE:
                if (doublePress) {
                    doublePress = false;
                    return true;
                }
                break;
            case State::SHIFT:
               return shiftPress;
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