#include "config/ConfigLoader.h"

StationConfig ConfigLoader::load() const
{
    StationConfig config{};
    config.station_id = "home_ref";

    SensorConfig sht41_config{};
    sht41_config.type = SensorType::SHT41;
    sht41_config.id = "sht41_main";
    sht41_config.sda_pin = 21;
    sht41_config.scl_pin = 22;
    sht41_config.i2c_address = 0x44;

    SensorConfig bmp280_config{};
    bmp280_config.type = SensorType::BMP280;
    bmp280_config.id = "bmp280_main";
    bmp280_config.sda_pin = 21;
    bmp280_config.scl_pin = 22;
    bmp280_config.i2c_address = 0x76;

    config.sensors.push_back(sht41_config);
    config.sensors.push_back(bmp280_config);

    return config;
}
