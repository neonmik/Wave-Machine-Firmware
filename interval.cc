#include "interval.h"

extern uint32_t sampleClock; // Declare the global sampleClock

namespace INTERVAL {
    void IntervalChecker::setInterval(uint32_t newInterval) {
        interval = newInterval;
    }

    bool IntervalChecker::checkInterval(void) {
        if ((!(sampleClock % interval)) && (sampleClock != last)) {
            last = sampleClock;
            return true;
        }
        return false;

    }

    IntervalChecker QUEUE(64);
    IntervalChecker UI(32);
}