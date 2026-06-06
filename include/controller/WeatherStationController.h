#pragma once

#include <cstdint>
#include <vector>

#include "battery/BatteryMonitor.h"
#include "logging/Logger.h"
#include "network/NetworkInfo.h"
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
            Publisher* publisher,
            Logger* logger,
            Clock* clock,
            NetworkInfo* network_info = nullptr,
            BatteryMonitor* battery_monitor = nullptr);

        bool initialize();
        bool tick();

        [[nodiscard]] int consecutivePublishFailures() const;
        [[nodiscard]] int consecutiveSampleFailures() const;
        [[nodiscard]] int totalSuccessfulPublishes() const;

    private:
        bool sampleSensors();
        bool publishBatch();

        const char* station_id_;
        unsigned long sample_interval_ms_;
        unsigned long publish_interval_ms_;

        std::vector<Sensor*> sensors_;
        Publisher* publisher_;
        Logger* logger_;
        NetworkInfo* network_info_;
        BatteryMonitor* battery_monitor_;

        std::uint32_t sequence_number_{0};

        unsigned long last_sample_ms_{0};
        unsigned long last_publish_ms_{0};

        std::vector<Observation> buffered_samples_;

        int consecutive_publish_failures_{0};
        int consecutive_sample_failures_{0};
        int total_successful_publishes_{0};

        Clock* clock_;
};
