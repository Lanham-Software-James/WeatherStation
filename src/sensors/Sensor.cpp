#include "sensors/Sensor.h"

bool Sensor::initialize()
{
    if (initialized_)
    {
        return true;
    }

    initialized_ = onInitialize();
    return initialized_;
}

bool Sensor::read(Observation& observation)
{
    if (!initialized_)
    {
        return false;
    }

    return onRead(observation);
}

bool Sensor::isInitialized() const noexcept
{
    return initialized_;
}
