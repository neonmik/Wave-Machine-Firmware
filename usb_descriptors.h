#pragma once

// #include "pico/stdlib.h"


extern bool USB_MODE;

bool getUSBMode(void) {
    return USB_MODE;
}

void setUSBMode(bool mode) {
    USB_MODE = mode;
}