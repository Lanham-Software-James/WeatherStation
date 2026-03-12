#pragma once

#include <ctime>
#include <vector>

#include "model/Observation.h"

struct ObservationBatch
{
    const char* station_id{nullptr};
    std::time_t sent_at{0};
    std::vector<Observation> samples;
};
