#include "doctest.h"

#include <string>

#include "model/Observation.h"
#include "../test/mocks/MockConstructedSensor.h"

TEST_CASE("MockConstructedSensor stores name and id from constructor")
{
    MockConstructedSensor sensor("TestSensor", "sensor_001");

    CHECK(std::string(sensor.getName()) == "TestSensor");
    CHECK(std::string(sensor.getId()) == "sensor_001");
}

TEST_CASE("MockConstructedSensor initialize succeeds")
{
    MockConstructedSensor sensor("TestSensor", "sensor_001");

    CHECK(sensor.initialize() == true);
    CHECK(sensor.isInitialized() == true);
}

TEST_CASE("MockConstructedSensor read succeeds after initialization")
{
    MockConstructedSensor sensor("TestSensor", "sensor_001");

    REQUIRE(sensor.initialize() == true);

    Observation obs{};

    CHECK(sensor.read(obs) == true);
}

TEST_CASE("MockConstructedSensor read fails before initialization")
{
    MockConstructedSensor sensor("TestSensor", "sensor_001");

    Observation obs{};

    CHECK(sensor.read(obs) == false);
}

TEST_CASE("MockConstructedSensor works through Sensor interface")
{
    MockConstructedSensor concrete("TestSensor", "sensor_001");
    Sensor* sensor = &concrete;

    REQUIRE(sensor->initialize() == true);

    Observation obs{};

    CHECK(sensor->read(obs) == true);
    CHECK(std::string(sensor->getName()) == "TestSensor");
    CHECK(std::string(sensor->getId()) == "sensor_001");
}

TEST_CASE("MockConstructedSensor does not modify observation data")
{
    MockConstructedSensor sensor("TestSensor", "sensor_001");

    REQUIRE(sensor.initialize() == true);

    Observation obs{};
    obs.temperature_c = 10.0f;
    obs.humidity_pct = 20.0f;
    obs.pressure_hpa = 1000.0f;

    REQUIRE(sensor.read(obs) == true);

    CHECK(obs.temperature_c == 10.0f);
    CHECK(obs.humidity_pct == 20.0f);
    CHECK(obs.pressure_hpa == 1000.0f);
}
