#include "doctest.h"

#include "../test/mocks/MockPublisher.h"
#include "model/Observation.h"
#include "model/ObservationBatch.h"

TEST_CASE("Publisher starts uninitialized")
{
    MockPublisher publisher;

    CHECK(publisher.isInitialized() == false);
}

TEST_CASE("Publisher publish fails before initialization")
{
    MockPublisher publisher;
    ObservationBatch batch{};

    const bool result = publisher.publish(batch);

    CHECK(result == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(publisher.on_publish_called == false);
}

TEST_CASE("Publisher initialize succeeds when onInitialize succeeds")
{
    MockPublisher publisher;

    const bool result = publisher.initialize();

    CHECK(result == true);
    CHECK(publisher.isInitialized() == true);
    CHECK(publisher.on_initialize_called == true);
}

TEST_CASE("Publisher initialize fails when onInitialize fails")
{
    MockPublisher publisher;
    publisher.initialize_result = false;

    const bool result = publisher.initialize();

    CHECK(result == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(publisher.on_initialize_called == true);
}

TEST_CASE("Publisher publish succeeds after successful initialization")
{
    MockPublisher publisher;

    Observation sample_one{};
    sample_one.station_id = "station-east-1";
    sample_one.timestamp_utc = 1700000000;
    sample_one.temperature_c = 24.5f;
    sample_one.humidity_pct = 58.0f;
    sample_one.pressure_hpa = 1012.8f;

    Observation sample_two{};
    sample_two.station_id = "station-east-1";
    sample_two.timestamp_utc = 1700000010;
    sample_two.temperature_c = 24.6f;
    sample_two.humidity_pct = 57.5f;
    sample_two.pressure_hpa = 1012.7f;

    ObservationBatch batch{};
    batch.station_id = "station-east-1";
    batch.sent_at = 1700000060;
    batch.samples.push_back(sample_one);
    batch.samples.push_back(sample_two);

    REQUIRE(publisher.initialize() == true);

    const bool result = publisher.publish(batch);

    CHECK(result == true);
    CHECK(publisher.on_publish_called == true);

    CHECK(publisher.last_batch.station_id == "station-east-1");
    CHECK(publisher.last_batch.sent_at == 1700000060);
    REQUIRE(publisher.last_batch.samples.size() == 2);

    CHECK(publisher.last_batch.samples[0].temperature_c == doctest::Approx(24.5f));
    CHECK(publisher.last_batch.samples[0].humidity_pct == doctest::Approx(58.0f));
    CHECK(publisher.last_batch.samples[0].pressure_hpa == doctest::Approx(1012.8f));

    CHECK(publisher.last_batch.samples[1].temperature_c == doctest::Approx(24.6f));
    CHECK(publisher.last_batch.samples[1].humidity_pct == doctest::Approx(57.5f));
    CHECK(publisher.last_batch.samples[1].pressure_hpa == doctest::Approx(1012.7f));
}

TEST_CASE("Publisher publish returns false when onPublish fails")
{
    MockPublisher publisher;
    publisher.publish_result = false;

    ObservationBatch batch{};

    REQUIRE(publisher.initialize() == true);

    const bool result = publisher.publish(batch);

    CHECK(result == false);
    CHECK(publisher.on_publish_called == true);
}

TEST_CASE("Publisher initialize does not reinitialize once already initialized")
{
    MockPublisher publisher;

    REQUIRE(publisher.initialize() == true);

    publisher.on_initialize_called = false;

    const bool result = publisher.initialize();

    CHECK(result == true);
    CHECK(publisher.isInitialized() == true);
    CHECK(publisher.on_initialize_called == false);
}
