#pragma once

#include <string>
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
    std::string id;
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
    std::string station_id{"default_station"};
    std::vector<SensorConfig> sensors;
};
