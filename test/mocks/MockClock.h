#pragma once

#include <ctime>

#include "time/Clock.h"

class MockClock : public Clock
{
    public:
        unsigned long current_millis{0};
        std::time_t current_time{0};

        [[nodiscard]] unsigned long millis() const override
        {
            return current_millis;
        }

        [[nodiscard]] std::time_t now() const override
        {
            return current_time;
        }

        void advanceMillis(unsigned long delta)
        {
            current_millis += delta;
        }

        void setMillis(unsigned long value)
        {
            current_millis = value;
        }

        void setNow(std::time_t value)
        {
            current_time = value;
        }
};
