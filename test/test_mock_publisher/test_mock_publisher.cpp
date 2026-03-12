#include "doctest.h"

#include <string>

#include "model/Observation.h"
#include "model/ObservationBatch.h"
#include "../test/mocks/MockPublisher.h"

namespace
{
    Observation makeObservation(
        const char* station_id,
        std::time_t ts,
        float temperature_c,
        float humidity_pct,
        float pressure_hpa)
    {
        Observation observation{};
        observation.station_id = station_id;
        observation.timestamp_utc = ts;
        observation.temperature_c = temperature_c;
        observation.humidity_pct = humidity_pct;
        observation.pressure_hpa = pressure_hpa;
        return observation;
    }
}

TEST_CASE("MockPublisher returns expected name")
{
    MockPublisher publisher;

    CHECK(std::string(publisher.getName()) == "MockPublisher");
}

TEST_CASE("MockPublisher starts uninitialized")
{
    MockPublisher publisher;

    CHECK(publisher.isInitialized() == false);
    CHECK(publisher.on_initialize_called == false);
    CHECK(publisher.on_publish_called == false);
}

TEST_CASE("MockPublisher initialize succeeds by default")
{
    MockPublisher publisher;

    CHECK(publisher.initialize() == true);
    CHECK(publisher.isInitialized() == true);
    CHECK(publisher.on_initialize_called == true);
}

TEST_CASE("MockPublisher initialize fails when initialize_result is false")
{
    MockPublisher publisher;
    publisher.initialize_result = false;

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(publisher.on_initialize_called == true);
}

TEST_CASE("MockPublisher publish fails before initialization")
{
    MockPublisher publisher;

    ObservationBatch batch{};
    batch.station_id = "station_001";
    batch.sent_at = 1700000060;

    CHECK(publisher.publish(batch) == false);
    CHECK(publisher.on_publish_called == false);
}

TEST_CASE("MockPublisher publish succeeds after initialization")
{
    MockPublisher publisher;

    ObservationBatch batch{};
    batch.station_id = "station_001";
    batch.sent_at = 1700000060;
    batch.samples.push_back(makeObservation("station_001", 1700000000, 21.5f, 48.0f, 1012.3f));
    batch.samples.push_back(makeObservation("station_001", 1700000010, 21.6f, 47.9f, 1012.2f));

    REQUIRE(publisher.initialize() == true);

    CHECK(publisher.publish(batch) == true);
    CHECK(publisher.on_publish_called == true);

    CHECK(std::string(publisher.last_batch.station_id) == "station_001");
    CHECK(publisher.last_batch.sent_at == 1700000060);

    REQUIRE(publisher.last_batch.samples.size() == 2);
    CHECK(publisher.last_batch.samples[0].timestamp_utc == 1700000000);
    CHECK(publisher.last_batch.samples[0].temperature_c == doctest::Approx(21.5f));
    CHECK(publisher.last_batch.samples[0].humidity_pct == doctest::Approx(48.0f));
    CHECK(publisher.last_batch.samples[0].pressure_hpa == doctest::Approx(1012.3f));

    CHECK(publisher.last_batch.samples[1].timestamp_utc == 1700000010);
    CHECK(publisher.last_batch.samples[1].temperature_c == doctest::Approx(21.6f));
    CHECK(publisher.last_batch.samples[1].humidity_pct == doctest::Approx(47.9f));
    CHECK(publisher.last_batch.samples[1].pressure_hpa == doctest::Approx(1012.2f));
}

TEST_CASE("MockPublisher publish returns false when publish_result is false")
{
    MockPublisher publisher;
    publisher.publish_result = false;

    ObservationBatch batch{};
    batch.station_id = "station_001";
    batch.sent_at = 1700000060;
    batch.samples.push_back(makeObservation("station_001", 1700000000, 21.5f, 48.0f, 1012.3f));

    REQUIRE(publisher.initialize() == true);

    CHECK(publisher.publish(batch) == false);
    CHECK(publisher.on_publish_called == true);
    CHECK(publisher.last_batch.station_id == nullptr);
    CHECK(publisher.last_batch.samples.empty());
}

TEST_CASE("MockPublisher initialize is idempotent after success")
{
    MockPublisher publisher;

    REQUIRE(publisher.initialize() == true);

    publisher.on_initialize_called = false;

    CHECK(publisher.initialize() == true);
    CHECK(publisher.isInitialized() == true);
    CHECK(publisher.on_initialize_called == false);
}

TEST_CASE("MockPublisher can be used through Publisher interface")
{
    MockPublisher concrete;
    Publisher* publisher = &concrete;

    ObservationBatch batch{};
    batch.station_id = "station_002";
    batch.sent_at = 1700000120;
    batch.samples.push_back(makeObservation("station_002", 1700000100, 19.8f, 55.0f, 1009.8f));

    REQUIRE(publisher->initialize() == true);
    CHECK(publisher->publish(batch) == true);
    CHECK(std::string(publisher->getName()) == "MockPublisher");

    CHECK(std::string(concrete.last_batch.station_id) == "station_002");
    REQUIRE(concrete.last_batch.samples.size() == 1);
    CHECK(concrete.last_batch.samples[0].temperature_c == doctest::Approx(19.8f));
}

TEST_CASE("MockPublisher copies batch data on successful publish")
{
    MockPublisher publisher;

    ObservationBatch batch{};
    batch.station_id = "station_003";
    batch.sent_at = 1700000180;
    batch.samples.push_back(makeObservation("station_003", 1700000150, 18.5f, 60.0f, 1008.5f));

    REQUIRE(publisher.initialize() == true);
    REQUIRE(publisher.publish(batch) == true);

    batch.station_id = "modified_station";
    batch.sent_at = 999;
    batch.samples[0].temperature_c = -99.0f;

    CHECK(std::string(publisher.last_batch.station_id) == "station_003");
    CHECK(publisher.last_batch.sent_at == 1700000180);
    REQUIRE(publisher.last_batch.samples.size() == 1);
    CHECK(publisher.last_batch.samples[0].temperature_c == doctest::Approx(18.5f));
}
