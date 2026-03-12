#include "doctest.h"

#include <string>

#include "model/Observation.h"
#include "model/ObservationBatch.h"

TEST_CASE("ObservationBatch initializes with default values")
{
    ObservationBatch batch{};

    CHECK(batch.station_id == nullptr);
    CHECK(batch.sent_at == 0);
    CHECK(batch.samples.empty());
}

TEST_CASE("ObservationBatch values can be assigned explicitly")
{
    ObservationBatch batch{};

    Observation sample_one{};
    sample_one.station_id = "station_001";
    sample_one.sequence_number = 1;
    sample_one.temperature_c = 21.5f;
    sample_one.humidity_pct = 48.1f;
    sample_one.pressure_hpa = 1012.3f;
    sample_one.timestamp_utc = 1700000000;

    Observation sample_two{};
    sample_two.station_id = "station_001";
    sample_two.sequence_number = 2;
    sample_two.temperature_c = 21.6f;
    sample_two.humidity_pct = 48.0f;
    sample_two.pressure_hpa = 1012.2f;
    sample_two.timestamp_utc = 1700000010;

    batch.station_id = "station_001";
    batch.sent_at = 1700000060;
    batch.samples.push_back(sample_one);
    batch.samples.push_back(sample_two);

    CHECK(std::string(batch.station_id) == "station_001");
    CHECK(batch.sent_at == 1700000060);

    REQUIRE(batch.samples.size() == 2);

    CHECK(std::string(batch.samples[0].station_id) == "station_001");
    CHECK(batch.samples[0].sequence_number == 1);
    CHECK(batch.samples[0].temperature_c == 21.5f);
    CHECK(batch.samples[0].humidity_pct == 48.1f);
    CHECK(batch.samples[0].pressure_hpa == 1012.3f);
    CHECK(batch.samples[0].timestamp_utc == 1700000000);

    CHECK(std::string(batch.samples[1].station_id) == "station_001");
    CHECK(batch.samples[1].sequence_number == 2);
    CHECK(batch.samples[1].temperature_c == 21.6f);
    CHECK(batch.samples[1].humidity_pct == 48.0f);
    CHECK(batch.samples[1].pressure_hpa == 1012.2f);
    CHECK(batch.samples[1].timestamp_utc == 1700000010);
}

TEST_CASE("ObservationBatch samples can be modified independently")
{
    ObservationBatch batch{};

    Observation sample{};
    sample.station_id = "station_001";
    sample.temperature_c = 22.0f;
    sample.humidity_pct = 50.0f;
    sample.pressure_hpa = 1010.0f;
    sample.timestamp_utc = 1700000000;

    batch.samples.push_back(sample);

    batch.samples[0].temperature_c = 23.4f;
    batch.samples[0].humidity_pct = 55.5f;

    REQUIRE(batch.samples.size() == 1);
    CHECK(batch.samples[0].temperature_c == 23.4f);
    CHECK(batch.samples[0].humidity_pct == 55.5f);
    CHECK(batch.samples[0].pressure_hpa == 1010.0f);
}

TEST_CASE("ObservationBatch can be copied")
{
    ObservationBatch original{};
    original.station_id = "station_002";
    original.sent_at = 1700000120;

    Observation sample{};
    sample.station_id = "station_002";
    sample.sequence_number = 3;
    sample.temperature_c = 19.8f;
    sample.humidity_pct = 60.0f;
    sample.pressure_hpa = 1009.5f;
    sample.timestamp_utc = 1700000100;

    original.samples.push_back(sample);

    ObservationBatch copy = original;

    CHECK(copy.station_id == original.station_id);
    CHECK(std::string(copy.station_id) == "station_002");
    CHECK(copy.sent_at == 1700000120);

    REQUIRE(copy.samples.size() == 1);
    CHECK(std::string(copy.samples[0].station_id) == "station_002");
    CHECK(copy.samples[0].sequence_number == 3);
    CHECK(copy.samples[0].temperature_c == 19.8f);
    CHECK(copy.samples[0].humidity_pct == 60.0f);
    CHECK(copy.samples[0].pressure_hpa == 1009.5f);
    CHECK(copy.samples[0].timestamp_utc == 1700000100);
}

TEST_CASE("ObservationBatch can be assigned")
{
    ObservationBatch source{};
    source.station_id = "station_003";
    source.sent_at = 1700000180;

    Observation sample{};
    sample.station_id = "station_003";
    sample.sequence_number = 4;
    sample.temperature_c = 18.5f;
    sample.humidity_pct = 65.0f;
    sample.pressure_hpa = 1008.8f;
    sample.timestamp_utc = 1700000150;

    source.samples.push_back(sample);

    ObservationBatch target{};
    target = source;

    CHECK(target.station_id == source.station_id);
    CHECK(std::string(target.station_id) == "station_003");
    CHECK(target.sent_at == 1700000180);

    REQUIRE(target.samples.size() == 1);
    CHECK(std::string(target.samples[0].station_id) == "station_003");
    CHECK(target.samples[0].sequence_number == 4);
    CHECK(target.samples[0].temperature_c == 18.5f);
    CHECK(target.samples[0].humidity_pct == 65.0f);
    CHECK(target.samples[0].pressure_hpa == 1008.8f);
    CHECK(target.samples[0].timestamp_utc == 1700000150);
}

TEST_CASE("ObservationBatch station_id can be set to null")
{
    ObservationBatch batch{};
    batch.station_id = nullptr;

    CHECK(batch.station_id == nullptr);
    CHECK(batch.sent_at == 0);
    CHECK(batch.samples.empty());
}

TEST_CASE("ObservationBatch can contain multiple samples in order")
{
    ObservationBatch batch{};
    batch.station_id = "station_004";
    batch.sent_at = 1700000300;

    Observation first{};
    first.station_id = "station_004";
    first.sequence_number = 1;
    first.timestamp_utc = 1700000010;

    Observation second{};
    second.station_id = "station_004";
    second.sequence_number = 2;
    second.timestamp_utc = 1700000020;

    Observation third{};
    third.station_id = "station_004";
    third.sequence_number = 3;
    third.timestamp_utc = 1700000030;

    batch.samples.push_back(first);
    batch.samples.push_back(second);
    batch.samples.push_back(third);

    REQUIRE(batch.samples.size() == 3);
    CHECK(batch.samples[0].sequence_number == 1);
    CHECK(batch.samples[1].sequence_number == 2);
    CHECK(batch.samples[2].sequence_number == 3);
}
