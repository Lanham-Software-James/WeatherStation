#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "model/Observation.h"

TEST_CASE("Observation initializes with default values") {
    Observation obs;

    CHECK(obs.station_id == "default_station");
    CHECK(obs.sequence_number == 0);

    CHECK(obs.temperature_c == 0.0f);
    CHECK(obs.humidity_pct == 0.0f);
    CHECK(obs.pressure_hpa == 0.0f);

    CHECK(obs.timestamp_utc == 0);
}

TEST_CASE("Observation values can be assigned explicitly") {
    Observation obs;

    obs.station_id = "station-east-2";
    obs.sequence_number = 42;
    obs.pressure_hpa = 1013.25f;
    obs.temperature_c = 25.5f;
    obs.humidity_pct = 60.0f;
    obs.timestamp_utc = 1700000000;

    CHECK(obs.station_id == "station-east-2");
    CHECK(obs.sequence_number == 42);

    CHECK(obs.pressure_hpa == 1013.25f);
    CHECK(obs.temperature_c == 25.5f);
    CHECK(obs.humidity_pct == 60.0f);

    CHECK(obs.timestamp_utc == 1700000000);
}

TEST_CASE("Observation fields can be modified independently") {
    Observation obs;

    obs.temperature_c = 21.4f;
    obs.humidity_pct = 48.0f;

    CHECK(obs.temperature_c == 21.4f);
    CHECK(obs.humidity_pct == 48.0f);

    CHECK(obs.pressure_hpa == 0.0f);
}
