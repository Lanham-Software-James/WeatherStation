#include "doctest.h"

#include "../test/mocks/MockSensor.h"
#include "model/Observation.h"

TEST_CASE("Sensor starts uninitialized")
{
    MockSensor sensor;

    CHECK(sensor.isInitialized() == false);
}

TEST_CASE("Sensor exposes expected name and id")
{
    MockSensor sensor;

    CHECK(sensor.getName() == "MockSensor");
    CHECK(sensor.getId() == "mock_sensor_1");
}

TEST_CASE("Sensor read fails before initialization")
{
    MockSensor sensor;
    Observation observation{};

    const bool result = sensor.read(observation);

    CHECK(result == false);
    CHECK(sensor.isInitialized() == false);
    CHECK(sensor.on_read_called == false);
}

TEST_CASE("Sensor initialize succeeds when onInitialize succeeds")
{
    MockSensor sensor;

    const bool result = sensor.initialize();

    CHECK(result == true);
    CHECK(sensor.isInitialized() == true);
    CHECK(sensor.on_initialize_called == true);
}

TEST_CASE("Sensor initialize fails when onInitialize fails")
{
    MockSensor sensor;
    sensor.initialize_result = false;

    const bool result = sensor.initialize();

    CHECK(result == false);
    CHECK(sensor.isInitialized() == false);
    CHECK(sensor.on_initialize_called == true);
}

TEST_CASE("Sensor read succeeds after successful initialization")
{
    MockSensor sensor;
    Observation observation{};

    REQUIRE(sensor.initialize() == true);

    const bool result = sensor.read(observation);

    CHECK(result == true);
    CHECK(sensor.on_read_called == true);
    CHECK(observation.temperature_c == 22.5f);
    CHECK(observation.humidity_pct == 50.0f);
    CHECK(observation.pressure_hpa == 1013.25f);
}

TEST_CASE("Sensor read returns false when onRead fails")
{
    MockSensor sensor;
    sensor.read_result = false;
    Observation observation{};

    REQUIRE(sensor.initialize() == true);

    const bool result = sensor.read(observation);

    CHECK(result == false);
    CHECK(sensor.on_read_called == true);
}

TEST_CASE("Sensor initialize does not reinitialize once already initialized")
{
    MockSensor sensor;

    REQUIRE(sensor.initialize() == true);
    sensor.on_initialize_called = false;

    const bool result = sensor.initialize();

    CHECK(result == true);
    CHECK(sensor.isInitialized() == true);
    CHECK(sensor.on_initialize_called == false);
}
