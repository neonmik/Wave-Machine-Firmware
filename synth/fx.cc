#include "fx.h"

namespace FX {
    namespace SOFTCLIP {
        void set_gain (uint16_t gain) {
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

            // need to figure out a reverse gain, so I can gain to saturation, but the volume wont get louder?
            // if (_gain > 2) sample /= _gain >> 1;
            // else sample /= _gain + (_gain >> 2);
        
            // sample /= _gain + (_gain >> (32 - _gain));
        
            // sample /= _gain;
        }
    }
    namespace HARDCLIP {
        void process (int32_t &sample) {
            sample <= -0x8000 ? -0x8000 : (sample > 0x7fff ? 0x7fff : sample);
        }
    }
}