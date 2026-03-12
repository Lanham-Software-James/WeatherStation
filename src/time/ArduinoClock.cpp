#include "time/ArduinoClock.h"

#include <Arduino.h>
#include <ctime>

unsigned long ArduinoClock::millis() const
{
    return ::millis();
}

std::time_t ArduinoClock::now() const
{
    return std::time(nullptr);
}
