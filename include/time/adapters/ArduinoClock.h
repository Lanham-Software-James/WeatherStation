#pragma once

#include "time/Clock.h"

class ArduinoClock : public Clock
{
    public:
        unsigned long millis() const override;
        std::time_t now() const override;
};
