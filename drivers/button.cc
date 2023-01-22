#include "button.h"


void Buttons::pressed () {
    shift_flag_ = !shift_flag_;
    start_ = to_ms_since_boot (get_absolute_time());
}
void Buttons::released () {
    shift_flag_ = !shift_flag_;
    end_ = to_ms_since_boot (get_absolute_time());
    // short press
    if (end_ - start_ < LONG_PRESS) {
        short_flag_ = true;
    }
    // long press
    if (end_ - start_ > LONG_PRESS) { 
        long_flag_ = true;
    }
    
}
bool Buttons::get_short () {
    bool temp = short_flag_;
    short_flag_ = false;
    return temp;
}
bool Buttons::get_long () {
    bool temp = long_flag_;
    long_flag_ = false;
    return temp;
}
bool Buttons::get_double () {
    bool temp = double_flag_;
    double_flag_ = false;
    return temp;
}
bool Buttons::get_shift () {
    return shift_flag_;
}