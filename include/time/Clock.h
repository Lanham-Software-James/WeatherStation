#pragma once

#include <ctime>

class Clock
{
    public:
        virtual ~Clock() = default;

        virtual unsigned long millis() const = 0;
        virtual std::time_t now() const = 0;
};
