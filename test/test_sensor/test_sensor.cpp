#include "doctest.h"

#include "../test/mocks/MockSensor.h"

TEST_CASE("Sensor starts uninitialized")
{
    FakeSensor sensor;

    CHECK(sensor.isInitialized() == false);
}

TEST_CASE("Sensor read fails before initialization")
{
    FakeSensor sensor;
    Observation observation;

    bool result = sensor.read(observation);

    CHECK(result == false);
    CHECK(sensor.isInitialized() == false);
    CHECK(sensor.on_read_called == false);
}

TEST_CASE("Sensor initialize succeeds when onInitialize succeeds")
{
    FakeSensor sensor;

    bool result = sensor.initialize();

    CHECK(result == true);
    CHECK(sensor.isInitialized() == true);
    CHECK(sensor.on_initialize_called == true);
}

TEST_CASE("Sensor initialize fails when onInitialize fails")
{
    FakeSensor sensor;
    sensor.initialize_result = false;

    bool result = sensor.initialize();

    CHECK(result == false);
    CHECK(sensor.isInitialized() == false);
    CHECK(sensor.on_initialize_called == true);
}

TEST_CASE("Sensor read succeeds after successful initialization")
{
    FakeSensor sensor;
    Observation observation;

    REQUIRE(sensor.initialize() == true);

    bool result = sensor.read(observation);

    CHECK(result == true);
    CHECK(sensor.on_read_called == true);
    CHECK(observation.temperature_c == 22.5f);
}

TEST_CASE("Sensor read returns false when onRead fails")
{
    FakeSensor sensor;
    sensor.read_result = false;
    Observation observation;

    REQUIRE(sensor.initialize() == true);

    bool result = sensor.read(observation);

    CHECK(result == false);
    CHECK(sensor.on_read_called == true);
}

TEST_CASE("Sensor initialize does not reinitialize once already initialized")
{
    FakeSensor sensor;

    REQUIRE(sensor.initialize() == true);
    sensor.on_initialize_called = false;

    bool result = sensor.initialize();

    CHECK(result == true);
    CHECK(sensor.isInitialized() == true);
    CHECK(sensor.on_initialize_called == false);
}
