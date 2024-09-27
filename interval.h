#pragma once

#include "config.h"


namespace INTERVAL {
    class IntervalChecker {
        private:
            uint32_t interval;
            uint32_t last;
        public:
            IntervalChecker(uint32_t interval = 0) : interval(interval), last(0) {}

            void setInterval(uint32_t newInterval);
            bool checkInterval(void);
    };

    extern IntervalChecker QUEUE;
    extern IntervalChecker UI;

}