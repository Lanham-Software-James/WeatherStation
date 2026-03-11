#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "model/Observation.h"
#include "sensors/Sensor.h"

class WeatherStationController
{
    public:
        explicit WeatherStationController(const char* station_id, std::vector<Sensor*> sensors);

        bool initialize();
        bool tick();

    private:
        const char* station_id_;
        std::vector<Sensor*> sensors_;

        std::int32_t sequence_number_{0};
};
