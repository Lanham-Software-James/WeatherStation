#include "publisher/Publisher.h"

bool Publisher::initialize()
{
    if (initialized_)
    {
        return true;
    }

    initialized_ = onInitialize();
    return initialized_;
}

bool Publisher::publish(const Observation& observation)
{
    if (!initialized_)
    {
        return false;
    }

    return onPublish(observation);
}

bool Publisher::isInitialized() const
{
    return initialized_;
}
