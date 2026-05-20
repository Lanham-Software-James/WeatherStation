#include <cstring>

#include "doctest.h"
#include "config/ConfigLoader.h"

TEST_CASE("ConfigLoader returns default station config")
{
    ConfigLoader loader;
    StationConfig config = loader.load();

    CHECK(std::strcmp(config.station_id, "station_001") == 0);
    REQUIRE(config.sensors.size() == 2);

    CHECK(config.sensors[0].type == SensorType::SHT41);
    CHECK(config.sensors[1].type == SensorType::BMP280);
}

TEST_CASE("ConfigLoader returns correct sample and publish intervals")
{
    ConfigLoader loader;
    StationConfig config = loader.load();

    CHECK(config.sample_interval_ms == 10000);
    CHECK(config.publish_interval_ms == 60000);
}

TEST_CASE("ConfigLoader returns sensor configuration details")
{
    ConfigLoader loader;
    StationConfig config = loader.load();

    REQUIRE(config.sensors.size() == 2);

    CHECK(std::strcmp(config.sensors[0].id, "sht41_main") == 0);
    CHECK(config.sensors[0].sda_pin == 21);
    CHECK(config.sensors[0].scl_pin == 22);
    CHECK(config.sensors[0].i2c_address == 0x44);
    CHECK(config.sensors[0].enabled == true);

    CHECK(std::strcmp(config.sensors[1].id, "bmp280_main") == 0);
    CHECK(config.sensors[1].sda_pin == 21);
    CHECK(config.sensors[1].scl_pin == 22);
    CHECK(config.sensors[1].i2c_address == 0x76);
    CHECK(config.sensors[1].enabled == true);
}

TEST_CASE("ConfigLoader returns publisher configuration")
{
    ConfigLoader loader;
    StationConfig config = loader.load();

    REQUIRE(config.publishers.size() == 1);
    CHECK(config.publishers[0].type == PublisherType::HTTP);
    CHECK(std::strcmp(config.publishers[0].id, "http_main") == 0);
    CHECK(config.publishers[0].enabled == true);
}
