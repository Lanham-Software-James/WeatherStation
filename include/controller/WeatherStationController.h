#pragma once

#include <cstdint>
#include <vector>

#include "logging/Logger.h"
#include "publisher/Publisher.h"
#include "sensors/Sensor.h"
#include "model/ObservationBatch.h"
#include "time/Clock.h"

class WeatherStationController
{
    public:
        WeatherStationController(
            const char* station_id,
            unsigned long sample_interval_ms,
            unsigned long publish_interval_ms,
            const std::vector<Sensor*>& sensors,
            const std::vector<Publisher*>& publishers,
            Logger* logger,
            Clock* clock);

        bool initialize();
        bool tick();

    private:
        bool sampleSensors();
        bool publishBatch();

        const char* station_id_;
        unsigned long sample_interval_ms_;
        unsigned long publish_interval_ms_;

        std::vector<Sensor*> sensors_;
        std::vector<Publisher*> publishers_;
        Logger* logger_;

        std::uint32_t sequence_number_{0};

        unsigned long last_sample_ms_{0};
        unsigned long last_publish_ms_{0};

        std::vector<Observation> buffered_samples_;

        Clock* clock_;
};
