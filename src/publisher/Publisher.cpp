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

bool Publisher::publish(const ObservationBatch& batch)
{
    if (!initialized_)
    {
        return false;
    }

    return onPublish(batch);
}

bool Publisher::isInitialized() const
{
    return initialized_;
}
