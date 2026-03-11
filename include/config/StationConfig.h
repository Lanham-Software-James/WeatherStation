#pragma once

#include <vector>
#include <cstdint>

enum class SensorType
{
    SHT41,
    BMP280
};

struct SensorConfig
{
    SensorType type;
    const char* id;
    bool enabled{true};
    int sda_pin{21};
    int scl_pin{22};
    std::uint8_t i2c_address{0x00};

    float temperature_offset_c{0.0f};
    float humidity_offset_pct{0.0f};
    float pressure_offset_hpa{0.0f};
};

struct StationConfig
{
    const char* station_id{"default_station"};
    std::vector<SensorConfig> sensors;
};
