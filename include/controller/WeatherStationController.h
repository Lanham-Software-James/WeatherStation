#pragma once

#include <vector>

#include "model/Observation.h"
#include "sensors/Sensor.h"

class WeatherStationController
{
    public:
        explicit WeatherStationController(std::string station_id, std::vector<Sensor*> sensors);

        bool initialize();
        void tick();

    private:
        std::vector<Sensor*> sensors_;
        std::string station_id_;
};
