#include "doctest.h"
#include "config/ConfigLoader.h"

TEST_CASE("ConfigLoader returns default station config")
{
    ConfigLoader loader;
    StationConfig config = loader.load();

    CHECK(config.station_id == "home_ref");
    REQUIRE(config.sensors.size() == 2);

    CHECK(config.sensors[0].type == SensorType::SHT41);
    CHECK(config.sensors[1].type == SensorType::BMP280);
}
