#include "doctest.h"

#include <string>

#include "model/Observation.h"
#include "../test/mocks/MockSensor.h"

TEST_CASE("MockSensor returns expected name and id")
{
    MockSensor sensor;

    CHECK(std::string(sensor.getName()) == "MockSensor");
    CHECK(std::string(sensor.getId()) == "mock_sensor_1");
}

TEST_CASE("MockSensor starts uninitialized")
{
    MockSensor sensor;

    CHECK(sensor.isInitialized() == false);
    CHECK(sensor.on_initialize_called == false);
    CHECK(sensor.on_read_called == false);
}

TEST_CASE("MockSensor initialize succeeds by default")
{
    MockSensor sensor;

    CHECK(sensor.initialize() == true);
    CHECK(sensor.isInitialized() == true);
    CHECK(sensor.on_initialize_called == true);
}

TEST_CASE("MockSensor initialize fails when initialize_result is false")
{
    MockSensor sensor;
    sensor.initialize_result = false;

    CHECK(sensor.initialize() == false);
    CHECK(sensor.isInitialized() == false);
    CHECK(sensor.on_initialize_called == true);
}

TEST_CASE("MockSensor read fails before initialization")
{
    MockSensor sensor;
    Observation observation{};

    CHECK(sensor.read(observation) == false);
    CHECK(sensor.on_read_called == false);
}

TEST_CASE("MockSensor read succeeds after initialization")
{
    MockSensor sensor;
    Observation observation{};

    REQUIRE(sensor.initialize() == true);

    CHECK(sensor.read(observation) == true);
    CHECK(sensor.on_read_called == true);

    CHECK(observation.temperature_c == doctest::Approx(22.5f));
    CHECK(observation.humidity_pct == doctest::Approx(50.0f));
    CHECK(observation.pressure_hpa == doctest::Approx(1013.25f));
}

TEST_CASE("MockSensor read returns false when read_result is false")
{
    MockSensor sensor;
    sensor.read_result = false;

    Observation observation{};
    observation.temperature_c = 1.0f;
    observation.humidity_pct = 2.0f;
    observation.pressure_hpa = 3.0f;

    REQUIRE(sensor.initialize() == true);

    CHECK(sensor.read(observation) == false);
    CHECK(sensor.on_read_called == true);

    CHECK(observation.temperature_c == doctest::Approx(1.0f));
    CHECK(observation.humidity_pct == doctest::Approx(2.0f));
    CHECK(observation.pressure_hpa == doctest::Approx(3.0f));
}

TEST_CASE("MockSensor writes configured values into observation")
{
    MockSensor sensor;
    sensor.temperature_to_write = 19.75f;
    sensor.humidity_to_write = 61.25f;
    sensor.pressure_to_write = 1008.40f;

    Observation observation{};

    REQUIRE(sensor.initialize() == true);
    REQUIRE(sensor.read(observation) == true);

    CHECK(observation.temperature_c == doctest::Approx(19.75f));
    CHECK(observation.humidity_pct == doctest::Approx(61.25f));
    CHECK(observation.pressure_hpa == doctest::Approx(1008.40f));
}

TEST_CASE("MockSensor initialize is idempotent after success")
{
    MockSensor sensor;

    REQUIRE(sensor.initialize() == true);

    sensor.on_initialize_called = false;

    CHECK(sensor.initialize() == true);
    CHECK(sensor.isInitialized() == true);
    CHECK(sensor.on_initialize_called == false);
}

TEST_CASE("MockSensor can be used through Sensor interface")
{
    MockSensor concrete;
    Sensor* sensor = &concrete;

    Observation observation{};

    REQUIRE(sensor->initialize() == true);
    REQUIRE(sensor->read(observation) == true);

    CHECK(std::string(sensor->getName()) == "MockSensor");
    CHECK(std::string(sensor->getId()) == "mock_sensor_1");
    CHECK(observation.temperature_c == doctest::Approx(22.5f));
    CHECK(observation.humidity_pct == doctest::Approx(50.0f));
    CHECK(observation.pressure_hpa == doctest::Approx(1013.25f));
}

TEST_CASE("MockSensor overwrites existing observation values on successful read")
{
    MockSensor sensor;

    Observation observation{};
    observation.temperature_c = -1.0f;
    observation.humidity_pct = -1.0f;
    observation.pressure_hpa = -1.0f;

    REQUIRE(sensor.initialize() == true);
    REQUIRE(sensor.read(observation) == true);

    CHECK(observation.temperature_c == doctest::Approx(22.5f));
    CHECK(observation.humidity_pct == doctest::Approx(50.0f));
    CHECK(observation.pressure_hpa == doctest::Approx(1013.25f));
}
