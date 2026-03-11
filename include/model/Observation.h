#pragma once

#include <cstdint>
#include <ctime>
#include <string>

struct Observation
{
    const char* station_id{"default_station"};
    std::uint32_t sequence_number{0};

    float pressure_hpa{0.0f};
    float temperature_c{0.0f};
    float humidity_pct{0.0f};

    std::time_t timestamp_utc{0};
};
