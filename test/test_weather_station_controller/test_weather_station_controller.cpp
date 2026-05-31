#include "doctest.h"

#include <vector>

#include "controller/WeatherStationController.h"
#include "../test/mocks/MockClock.h"
#include "../test/mocks/MockLogger.h"
#include "../test/mocks/MockNetworkInfo.h"
#include "../test/mocks/MockPublisher.h"
#include "../test/mocks/MockSensor.h"

TEST_CASE("Controller initialize fails when station_id is null")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        nullptr,
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("no sensors provided"));
}

TEST_CASE("Controller initialize fails when publisher is null")
{
    MockSensor sensor;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        nullptr,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("publisher is null"));
}

TEST_CASE("Controller initialize fails when sensor is null")
{
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{nullptr};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("null sensor"));
}

TEST_CASE("Controller initialize fails when sensor initialization fails")
{
    MockSensor sensor;
    sensor.initialize_result = false;

    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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

    WeatherStationController controller(
        "station_001",
        100000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.advanceMillis(60000);
    clock.setNow(1700000060);

    CHECK(controller.tick() == true);
    CHECK(publisher.on_publish_called == false);
    CHECK(logger.contains("No samples collected; skipping publish"));
}

TEST_CASE("Controller initialize fails when sample interval is zero")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        0,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("sample interval must be > 0"));
}

TEST_CASE("Controller initialize fails when publish interval is zero")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        0,
        sensors,
        &publisher,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("publish interval must be > 0"));
}

TEST_CASE("Controller initialize fails when clock is null")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        nullptr);

    CHECK(controller.initialize() == false);
    CHECK(logger.contains("clock is null"));
}

TEST_CASE("Controller initialize fails when second sensor fails to initialize")
{
    MockSensor sensor1;
    MockSensor sensor2;
    sensor2.initialize_result = false;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor1, &sensor2};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    CHECK(controller.initialize() == false);
    CHECK(sensor1.on_initialize_called == true);
    CHECK(sensor2.on_initialize_called == true);
    CHECK(logger.contains("Failed to initialize sensor"));
}

TEST_CASE("Controller tick fails when second sensor read fails")
{
    MockSensor sensor1;
    MockSensor sensor2;
    sensor2.read_result = false;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);

    std::vector<Sensor*> sensors{&sensor1, &sensor2};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.setMillis(10000);

    CHECK(controller.tick() == false);
    CHECK(sensor1.on_read_called == true);
    CHECK(sensor2.on_read_called == true);
    CHECK(logger.contains("Failed to read sensor"));
}

TEST_CASE("Controller assigns incrementing sequence numbers to observations")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    for (int i = 0; i < 6; ++i)
    {
        clock.advanceMillis(10000);
        clock.setNow(1700000000 + ((i + 1) * 10));
        REQUIRE(controller.tick() == true);
    }

    REQUIRE(publisher.last_batch.samples.size() == 6);
    CHECK(publisher.last_batch.samples[0].sequence_number == 1);
    CHECK(publisher.last_batch.samples[1].sequence_number == 2);
    CHECK(publisher.last_batch.samples[2].sequence_number == 3);
    CHECK(publisher.last_batch.samples[3].sequence_number == 4);
    CHECK(publisher.last_batch.samples[4].sequence_number == 5);
    CHECK(publisher.last_batch.samples[5].sequence_number == 6);
}

TEST_CASE("Controller consecutive sample failure count starts at zero after initialization")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    CHECK(controller.consecutiveSampleFailures() == 0);
}

TEST_CASE("Controller consecutive sample failure count increments on successive failures")
{
    MockSensor sensor;
    sensor.read_result = false;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.setMillis(10000);
    CHECK(controller.tick() == false);
    CHECK(controller.consecutiveSampleFailures() == 1);

    clock.setMillis(20000);
    CHECK(controller.tick() == false);
    CHECK(controller.consecutiveSampleFailures() == 2);
}

TEST_CASE("Controller consecutive sample failure count resets after successful sample")
{
    MockSensor sensor;
    sensor.read_result = false;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.setMillis(10000);
    CHECK(controller.tick() == false);
    CHECK(controller.consecutiveSampleFailures() == 1);

    sensor.read_result = true;
    clock.setMillis(20000);
    CHECK(controller.tick() == true);
    CHECK(controller.consecutiveSampleFailures() == 0);
}

TEST_CASE("Controller consecutive publish failure count starts at zero after initialization")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    CHECK(controller.consecutivePublishFailures() == 0);
}

TEST_CASE("Controller consecutive publish failure count increments on successive failures")
{
    MockSensor sensor;
    MockPublisher publisher;
    publisher.publish_result = false;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.setMillis(60000);
    clock.setNow(1700000060);
    CHECK(controller.tick() == false);
    CHECK(controller.consecutivePublishFailures() == 1);

    clock.setMillis(120000);
    clock.setNow(1700000120);
    CHECK(controller.tick() == false);
    CHECK(controller.consecutivePublishFailures() == 2);
}

TEST_CASE("Controller consecutive publish failure count resets after successful publish")
{
    MockSensor sensor;
    MockPublisher publisher;
    publisher.publish_result = false;
    MockLogger logger;
    MockClock clock;
    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock);

    REQUIRE(controller.initialize() == true);

    clock.setMillis(60000);
    clock.setNow(1700000060);
    CHECK(controller.tick() == false);
    CHECK(controller.consecutivePublishFailures() == 1);

    publisher.publish_result = true;
    clock.setMillis(120000);
    clock.setNow(1700000120);
    CHECK(controller.tick() == true);
    CHECK(controller.consecutivePublishFailures() == 0);
}

TEST_CASE("Controller populates rssi_dbm in published batch when network_info provided")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    MockNetworkInfo network_info;
    network_info.rssi_to_return = -72;

    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock,
        &network_info);

    REQUIRE(controller.initialize() == true);

    for (int i = 0; i < 6; ++i)
    {
        clock.advanceMillis(10000);
        clock.setNow(1700000000 + ((i + 1) * 10));
        REQUIRE(controller.tick() == true);
    }

    CHECK(publisher.on_publish_called == true);
    CHECK(publisher.last_batch.rssi_dbm == -72);
}

TEST_CASE("Controller reads rssi_dbm at publish time not at sample time")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    MockNetworkInfo network_info;
    network_info.rssi_to_return = -50;

    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock,
        &network_info);

    REQUIRE(controller.initialize() == true);

    // Collect samples with RSSI at -50
    for (int i = 0; i < 3; ++i)
    {
        clock.advanceMillis(10000);
        clock.setNow(1700000000 + ((i + 1) * 10));
        REQUIRE(controller.tick() == true);
    }

    // RSSI changes before publish
    network_info.rssi_to_return = -88;

    for (int i = 3; i < 6; ++i)
    {
        clock.advanceMillis(10000);
        clock.setNow(1700000000 + ((i + 1) * 10));
        REQUIRE(controller.tick() == true);
    }

    // Batch should carry the publish-time RSSI, not the sample-time value
    CHECK(publisher.on_publish_called == true);
    CHECK(publisher.last_batch.rssi_dbm == -88);
}

TEST_CASE("Controller reads rssi_dbm fresh on each publish cycle")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;
    MockNetworkInfo network_info;
    network_info.rssi_to_return = -60;

    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
        &logger,
        &clock,
        &network_info);

    REQUIRE(controller.initialize() == true);

    // First publish cycle
    for (int i = 0; i < 6; ++i)
    {
        clock.advanceMillis(10000);
        clock.setNow(1700000000 + ((i + 1) * 10));
        REQUIRE(controller.tick() == true);
    }

    REQUIRE(publisher.on_publish_called == true);
    CHECK(publisher.last_batch.rssi_dbm == -60);

    // RSSI degrades before second publish cycle
    network_info.rssi_to_return = -80;

    for (int i = 6; i < 12; ++i)
    {
        clock.advanceMillis(10000);
        clock.setNow(1700000000 + ((i + 1) * 10));
        REQUIRE(controller.tick() == true);
    }

    CHECK(publisher.last_batch.rssi_dbm == -80);
}

TEST_CASE("Controller sets rssi_dbm to zero in batch when no network_info provided")
{
    MockSensor sensor;
    MockPublisher publisher;
    MockLogger logger;
    MockClock clock;

    clock.setMillis(0);
    clock.setNow(1700000000);

    std::vector<Sensor*> sensors{&sensor};

    WeatherStationController controller(
        "station_001",
        10000,
        60000,
        sensors,
        &publisher,
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
    CHECK(publisher.last_batch.rssi_dbm == 0);
}
