#include "doctest.h"

#include <vector>

#include "controller/WeatherStationController.h"
#include "../test/mocks/MockClock.h"
#include "../test/mocks/MockLogger.h"
#include "../test/mocks/MockPublisher.h"
#include "../test/mocks/MockSensor.h"

TEST_CASE("Controller initialize fails when station_id is null")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        nullptr,
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("station_id is null"));
}

TEST_CASE("Controller initialize fails when logger is null")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        nullptr,
        &clock);

    CHECK(controller.initialize() == false);
}

TEST_CASE("Controller initialize fails when no sensors are provided")
{
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("no sensors provided"));
}

TEST_CASE("Controller initialize fails when no publishers are provided")
{
    MockSensor sensor;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("no publishers provided"));
}

TEST_CASE("Controller initialize fails when sensor is null")
{
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{nullptr};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("null sensor"));
}

TEST_CASE("Controller initialize fails when publisher is null")
{
    MockSensor sensor;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{nullptr};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("null publisher"));
}

TEST_CASE("Controller initialize fails when sensor initialization fails")
{
    MockSensor sensor;
    sensor.initialize_result = false;

    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(sensor.on_initialize_called == true);
    CHECK(logger.contains("Failed to initialize sensor"));
}

TEST_CASE("Controller initialize fails when publisher initialization fails")
{
    MockSensor sensor;
    MockPublisher publisher;
    publisher.initialize_result = false;

    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(sensor.on_initialize_called == true);
    CHECK(publisher.on_initialize_called == true);
    CHECK(logger.contains("Failed to initialize publisher"));
}

TEST_CASE("Controller initialize succeeds with valid dependencies")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(5000);

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    CHECK(controller.initialize() == true);
    CHECK(sensor.on_initialize_called == true);
    CHECK(publisher.on_initialize_called == true);
    CHECK(logger.contains("Controller initialized successfully"));
}

TEST_CASE("Controller tick does nothing before sample interval elapses")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    CHECK(controller.tick() == true);
    CHECK(sensor.on_read_called == false);
    CHECK(publisher.on_publish_called == false);
}

TEST_CASE("Controller samples once after sample interval elapses")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.advanceMillis(10000);

    CHECK(controller.tick() == true);
    CHECK(sensor.on_read_called == true);
    CHECK(publisher.on_publish_called == false);
    CHECK(logger.contains("Sample collected"));
}

TEST_CASE("Controller publishes batch after publish interval elapses")
{
    MockSensor sensor;
    sensor.temperature_to_write = 21.5f;
    sensor.humidity_to_write = 48.0f;
    sensor.pressure_to_write = 1012.3f;

    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    for (int i = 0; i < 6; ++i)
    {
        clock.advanceMillis(10000);
        clock.setNow(1700000000 + ((i + 1) * 10));
        REQUIRE(controller.tick() == true);
    }

    CHECK(publisher.on_publish_called == true);
    CHECK(publisher.last_batch.station_id == std::string("station_001"));
    CHECK(publisher.last_batch.sent_at == 1700000060);
    REQUIRE(publisher.last_batch.samples.size() == 6);

    CHECK(publisher.last_batch.samples[0].temperature_c == doctest::Approx(21.5f));
    CHECK(publisher.last_batch.samples[0].humidity_pct == doctest::Approx(48.0f));
    CHECK(publisher.last_batch.samples[0].pressure_hpa == doctest::Approx(1012.3f));
}

TEST_CASE("Controller tick fails when sensor read fails")
{
    MockSensor sensor;
    sensor.read_result = false;

    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.advanceMillis(10000);

    CHECK(controller.tick() == false);
    CHECK(logger.contains("Failed to read sensor"));
    CHECK(publisher.on_publish_called == false);
}

TEST_CASE("Controller tick fails when publisher publish fails")
{
    MockSensor sensor;
    MockPublisher publisher;
    publisher.publish_result = false;

    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    for (int i = 0; i < 6; ++i)
    {
        clock.advanceMillis(10000);
        clock.setNow(1700000000 + ((i + 1) * 10));
    }

    CHECK(controller.tick() == false);
    CHECK(logger.contains("Failed to publish batch with publisher"));
}

TEST_CASE("Controller catches up on missed sample intervals")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.advanceMillis(30000);
    clock.setNow(1700000030);

    CHECK(controller.tick() == true);
    CHECK(publisher.on_publish_called == false);
}

TEST_CASE("Controller skips publishing when no samples have been collected")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};
    std::vector<Publisher*> publishers{&publisher};

    WeatherStationController controller(
        "station_001",
        100000,
        60000,
        sensors,
        publishers,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.advanceMillis(60000);
    clock.setNow(1700000060);

    CHECK(controller.tick() == true);
    CHECK(publisher.on_publish_called == false);
    CHECK(logger.contains("No samples collected; skipping publish"));
}
