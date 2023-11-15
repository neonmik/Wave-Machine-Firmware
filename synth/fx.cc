#include "fx.h"

namespace FX {
    namespace SOFTCLIP {
        void setGain (uint16_t gain) {
            _gain = (gain >> 5); // sets gain range from 1 - 31 (0 never outpus anything, 32+ overloads and never stops sustaining)
            if (_gain <=0) _gain = 1;
        }
        void process(int32_t &sample) {
            
            sample *= _gain; //between 1 and 31
            
            if (sample > _max) {
                sample = _max;
            }
            else if (sample < _min) {
                sample = _min;
            }
            else {
                const int32_t x = sample * sample / _threshold;
                sample = (3 * sample - (x * sample) / _threshold) / 2;
            }
        }
    }
    namespace HARDCLIP {
        void process16 (int32_t &sample) {
            sample <= INT16_MIN ? INT16_MIN : (sample > INT16_MAX ? INT16_MAX : sample);
        }
        void process (int32_t &sample) {
            sample <= INT32_MIN ? INT32_MIN : (sample > INT32_MAX ? INT32_MAX : sample);
        }
    }
}