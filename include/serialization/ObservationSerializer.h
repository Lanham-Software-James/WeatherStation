#pragma once

#include "model/ObservationBatch.h"

class ObservationSerializer
{
    public:
        [[nodiscard]] static const char* toJson(const ObservationBatch& batch);
};
