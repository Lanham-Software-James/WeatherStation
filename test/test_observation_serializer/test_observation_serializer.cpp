#include "doctest.h"

#include <cstring>
#include <string>

#include "model/Observation.h"
#include "serialization/ObservationSerializer.h"

TEST_CASE("ObservationSerializer serializes a complete observation to JSON")
{
    Observation observation{};
    observation.station_id = "home_ref";
    observation.sequence_number = 42;
    observation.timestamp_utc = 1700000000;
    observation.temperature_c = 21.52f;
    observation.humidity_pct = 48.10f;
    observation.pressure_hpa = 1012.32f;

    const char* json = ObservationSerializer::toJson(observation);

    REQUIRE(json != nullptr);

    CHECK(std::string(json) ==
          "{\"station_id\":\"home_ref\","
          "\"sequence_number\":42,"
          "\"timestamp_utc\":1700000000,"
          "\"temperature_c\":21.52,"
          "\"humidity_pct\":48.10,"
          "\"pressure_hpa\":1012.32}");
}

TEST_CASE("ObservationSerializer handles null station_id")
{
    Observation observation{};
    observation.station_id = nullptr;
    observation.sequence_number = 1;
    observation.timestamp_utc = 1000;
    observation.temperature_c = 10.0f;
    observation.humidity_pct = 20.0f;
    observation.pressure_hpa = 1010.0f;

    const char* json = ObservationSerializer::toJson(observation);

    REQUIRE(json != nullptr);

    CHECK(std::string(json) ==
          "{\"station_id\":\"\","
          "\"sequence_number\":1,"
          "\"timestamp_utc\":1000,"
          "\"temperature_c\":10.00,"
          "\"humidity_pct\":20.00,"
          "\"pressure_hpa\":1010.00}");
}

TEST_CASE("ObservationSerializer formats floating point values to two decimals")
{
    Observation observation{};
    observation.station_id = "station_a";
    observation.sequence_number = 7;
    observation.timestamp_utc = 1234567890;
    observation.temperature_c = 1.2f;
    observation.humidity_pct = 3.456f;
    observation.pressure_hpa = 999.0f;

    const char* json = ObservationSerializer::toJson(observation);

    REQUIRE(json != nullptr);

    CHECK(std::strstr(json, "\"temperature_c\":1.20") != nullptr);
    CHECK(std::strstr(json, "\"humidity_pct\":3.46") != nullptr);
    CHECK(std::strstr(json, "\"pressure_hpa\":999.00") != nullptr);
}

TEST_CASE("ObservationSerializer includes all expected JSON keys")
{
    Observation observation{};
    observation.station_id = "station_b";
    observation.sequence_number = 9;
    observation.timestamp_utc = 2000;
    observation.temperature_c = 25.0f;
    observation.humidity_pct = 50.0f;
    observation.pressure_hpa = 1013.25f;

    const char* json = ObservationSerializer::toJson(observation);

    REQUIRE(json != nullptr);

    CHECK(std::strstr(json, "\"station_id\"") != nullptr);
    CHECK(std::strstr(json, "\"sequence_number\"") != nullptr);
    CHECK(std::strstr(json, "\"timestamp_utc\"") != nullptr);
    CHECK(std::strstr(json, "\"temperature_c\"") != nullptr);
    CHECK(std::strstr(json, "\"humidity_pct\"") != nullptr);
    CHECK(std::strstr(json, "\"pressure_hpa\"") != nullptr);
}

TEST_CASE("ObservationSerializer reuses a stable buffer for consecutive calls")
{
    Observation first{};
    first.station_id = "station_one";
    first.sequence_number = 1;
    first.timestamp_utc = 111;
    first.temperature_c = 11.0f;
    first.humidity_pct = 22.0f;
    first.pressure_hpa = 1001.0f;

    Observation second{};
    second.station_id = "station_two";
    second.sequence_number = 2;
    second.timestamp_utc = 222;
    second.temperature_c = 33.0f;
    second.humidity_pct = 44.0f;
    second.pressure_hpa = 1002.0f;

    const char* first_json = ObservationSerializer::toJson(first);
    REQUIRE(first_json != nullptr);

    std::string first_copy = first_json;

    const char* second_json = ObservationSerializer::toJson(second);
    REQUIRE(second_json != nullptr);

    CHECK(first_copy != std::string(second_json));
    CHECK(std::string(second_json).find("station_two") != std::string::npos);
}
