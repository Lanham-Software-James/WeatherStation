#pragma once

#include "model/Observation.h"

class ObservationSerializer
{
    public:
        [[nodiscard]] static const char* toJson(const Observation& observation);
};
