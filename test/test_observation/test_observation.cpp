#include "doctest.h"

#include <string>

#include "model/Observation.h"

TEST_CASE("Observation initializes with default values")
{
    Observation obs;

    CHECK(std::string(obs.station_id) == "default_station");
    CHECK(obs.sequence_number == 0);

    CHECK(obs.temperature_c == 0.0f);
    CHECK(obs.humidity_pct == 0.0f);
    CHECK(obs.pressure_hpa == 0.0f);

    CHECK(obs.timestamp_utc == 0);
}

TEST_CASE("Observation values can be assigned explicitly")
{
    Observation obs;

    obs.station_id = "station-east-2";
    obs.sequence_number = 42;
    obs.pressure_hpa = 1013.25f;
    obs.temperature_c = 25.5f;
    obs.humidity_pct = 60.0f;
    obs.timestamp_utc = 1700000000;

    CHECK(std::string(obs.station_id) == "station-east-2");
    CHECK(obs.sequence_number == 42);

    CHECK(obs.pressure_hpa == 1013.25f);
    CHECK(obs.temperature_c == 25.5f);
    CHECK(obs.humidity_pct == 60.0f);

    CHECK(obs.timestamp_utc == 1700000000);
}

TEST_CASE("Observation fields can be modified independently")
{
    Observation obs;

    obs.temperature_c = 21.4f;
    obs.humidity_pct = 48.0f;

    CHECK(obs.temperature_c == 21.4f);
    CHECK(obs.humidity_pct == 48.0f);

    CHECK(obs.pressure_hpa == 0.0f);
    CHECK(obs.sequence_number == 0);
    CHECK(obs.timestamp_utc == 0);
}

TEST_CASE("Observation value-initializes correctly with braces")
{
    Observation obs{};

    CHECK(std::string(obs.station_id) == "default_station");
    CHECK(obs.sequence_number == 0);
    CHECK(obs.pressure_hpa == 0.0f);
    CHECK(obs.temperature_c == 0.0f);
    CHECK(obs.humidity_pct == 0.0f);
    CHECK(obs.timestamp_utc == 0);
}

TEST_CASE("Observation can be copied")
{
    Observation original{};
    original.station_id = "station_001";
    original.sequence_number = 7;
    original.pressure_hpa = 1012.8f;
    original.temperature_c = 21.5f;
    original.humidity_pct = 48.1f;
    original.timestamp_utc = 1700000000;

    Observation copy = original;

    CHECK(copy.station_id == original.station_id);
    CHECK(std::string(copy.station_id) == "station_001");
    CHECK(copy.sequence_number == 7);
    CHECK(copy.pressure_hpa == 1012.8f);
    CHECK(copy.temperature_c == 21.5f);
    CHECK(copy.humidity_pct == 48.1f);
    CHECK(copy.timestamp_utc == 1700000000);
}

TEST_CASE("Observation can be assigned")
{
    Observation source{};
    source.station_id = "station_002";
    source.sequence_number = 10;
    source.pressure_hpa = 1009.4f;
    source.temperature_c = 18.2f;
    source.humidity_pct = 55.0f;
    source.timestamp_utc = 1700000100;

    Observation target{};
    target = source;

    CHECK(target.station_id == source.station_id);
    CHECK(std::string(target.station_id) == "station_002");
    CHECK(target.sequence_number == 10);
    CHECK(target.pressure_hpa == 1009.4f);
    CHECK(target.temperature_c == 18.2f);
    CHECK(target.humidity_pct == 55.0f);
    CHECK(target.timestamp_utc == 1700000100);
}

TEST_CASE("Observation station_id can be set to null")
{
    Observation obs{};
    obs.station_id = nullptr;

    CHECK(obs.station_id == nullptr);
    CHECK(obs.sequence_number == 0);
    CHECK(obs.pressure_hpa == 0.0f);
    CHECK(obs.temperature_c == 0.0f);
    CHECK(obs.humidity_pct == 0.0f);
    CHECK(obs.timestamp_utc == 0);
}
