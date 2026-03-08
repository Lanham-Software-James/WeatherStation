#pragma once

#include <vector>

#include "config/StationConfig.h"
#include "sensors/Sensor.h"

class SensorFactory
{
    public:
        std::vector<Sensor*> createSensors(const StationConfig& config);
};
