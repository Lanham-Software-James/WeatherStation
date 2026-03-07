#include "doctest.h"

#include "../test/mocks/MockPublisher.h"
#include "model/Observation.h"

TEST_CASE("Publisher starts uninitialized")
{
    FakePublisher publisher;

    CHECK(publisher.isInitialized() == false);
}

TEST_CASE("Publisher publish fails before initialization")
{
    FakePublisher publisher;
    Observation observation{};

    bool result = publisher.publish(observation);

    CHECK(result == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(publisher.on_publish_called == false);
}

TEST_CASE("Publisher initialize succeeds when onInitialize succeeds")
{
    FakePublisher publisher;

    bool result = publisher.initialize();

    CHECK(result == true);
    CHECK(publisher.isInitialized() == true);
    CHECK(publisher.on_initialize_called == true);
}

TEST_CASE("Publisher initialize fails when onInitialize fails")
{
    FakePublisher publisher;
    publisher.initialize_result = false;

    bool result = publisher.initialize();

    CHECK(result == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(publisher.on_initialize_called == true);
}

TEST_CASE("Publisher publish succeeds after successful initialization")
{
    FakePublisher publisher;
    Observation observation{};

    observation.station_id = "station-east-1";
    observation.sequence_number = 7;
    observation.temperature_c = 24.5f;
    observation.humidity_pct = 58.0f;
    observation.pressure_hpa = 1012.8f;
    observation.timestamp_utc = 1700000000;

    REQUIRE(publisher.initialize() == true);

    bool result = publisher.publish(observation);

    CHECK(result == true);
    CHECK(publisher.on_publish_called == true);

    CHECK(publisher.last_observation.station_id == "station-east-1");
    CHECK(publisher.last_observation.sequence_number == 7);
    CHECK(publisher.last_observation.temperature_c == 24.5f);
    CHECK(publisher.last_observation.humidity_pct == 58.0f);
    CHECK(publisher.last_observation.pressure_hpa == 1012.8f);
    CHECK(publisher.last_observation.timestamp_utc == 1700000000);
}

TEST_CASE("Publisher publish returns false when onPublish fails")
{
    FakePublisher publisher;
    publisher.publish_result = false;

    Observation observation{};

    REQUIRE(publisher.initialize() == true);

    bool result = publisher.publish(observation);

    CHECK(result == false);
    CHECK(publisher.on_publish_called == true);
}

TEST_CASE("Publisher initialize does not reinitialize once already initialized")
{
    FakePublisher publisher;

    REQUIRE(publisher.initialize() == true);

    publisher.on_initialize_called = false;

    bool result = publisher.initialize();

    CHECK(result == true);
    CHECK(publisher.isInitialized() == true);
    CHECK(publisher.on_initialize_called == false);
}
