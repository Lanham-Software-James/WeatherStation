#include "doctest.h"

#include <cstring>
#include <string>

#include "model/Observation.h"
#include "model/ObservationBatch.h"
#include "serialization/ObservationSerializer.h"

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

TEST_CASE("ObservationSerializer serializes a complete batch to JSON")
{
    ObservationBatch batch{};
    batch.station_id = "home_ref";
    batch.sent_at = 1700000060;
    batch.samples.push_back(makeObservation("home_ref", 1700000000, 21.52f, 48.10f, 1012.32f));
    batch.samples.push_back(makeObservation("home_ref", 1700000010, 21.60f, 48.00f, 1012.28f));

    const char* json = ObservationSerializer::toJson(batch);

    REQUIRE(json != nullptr);

    CHECK(std::strstr(json, "\"station_id\":\"home_ref\"") != nullptr);
    CHECK(std::strstr(json, "\"sent_at\":\"") != nullptr);
    CHECK(std::strstr(json, "\"samples\":[") != nullptr);
    CHECK(std::strstr(json, "\"ts\":\"") != nullptr);
    CHECK(std::strstr(json, "\"temperature_c\":21.52") != nullptr);
    CHECK(std::strstr(json, "\"humidity_pct\":48.10") != nullptr);
    CHECK(std::strstr(json, "\"pressure_hpa\":1012.32") != nullptr);
}

TEST_CASE("ObservationSerializer handles null station_id in batch")
{
    ObservationBatch batch{};
    batch.station_id = nullptr;
    batch.sent_at = 1700000060;
    batch.samples.push_back(makeObservation(nullptr, 1700000000, 10.0f, 20.0f, 1010.0f));

    const char* json = ObservationSerializer::toJson(batch);

    REQUIRE(json != nullptr);

    CHECK(std::strstr(json, "\"station_id\":\"\"") != nullptr);
}

TEST_CASE("ObservationSerializer formats floating point values to two decimals in batch")
{
    ObservationBatch batch{};
    batch.station_id = "station_a";
    batch.sent_at = 1700000060;
    batch.samples.push_back(makeObservation("station_a", 1234567890, 1.2f, 3.456f, 999.0f));

    const char* json = ObservationSerializer::toJson(batch);

    REQUIRE(json != nullptr);

    CHECK(std::strstr(json, "\"temperature_c\":1.20") != nullptr);
    CHECK(std::strstr(json, "\"humidity_pct\":3.46") != nullptr);
    CHECK(std::strstr(json, "\"pressure_hpa\":999.00") != nullptr);
}

TEST_CASE("ObservationSerializer includes all expected JSON keys for batch")
{
    ObservationBatch batch{};
    batch.station_id = "station_b";
    batch.sent_at = 1700000060;
    batch.samples.push_back(makeObservation("station_b", 2000, 25.0f, 50.0f, 1013.25f));

    const char* json = ObservationSerializer::toJson(batch);

    REQUIRE(json != nullptr);

    CHECK(std::strstr(json, "\"station_id\"") != nullptr);
    CHECK(std::strstr(json, "\"sent_at\"") != nullptr);
    CHECK(std::strstr(json, "\"samples\"") != nullptr);
    CHECK(std::strstr(json, "\"ts\"") != nullptr);
    CHECK(std::strstr(json, "\"temperature_c\"") != nullptr);
    CHECK(std::strstr(json, "\"humidity_pct\"") != nullptr);
    CHECK(std::strstr(json, "\"pressure_hpa\"") != nullptr);
}

TEST_CASE("ObservationSerializer reuses a stable buffer for consecutive calls")
{
    ObservationBatch first{};
    first.station_id = "station_one";
    first.sent_at = 111;
    first.samples.push_back(makeObservation("station_one", 101, 11.0f, 22.0f, 1001.0f));

    ObservationBatch second{};
    second.station_id = "station_two";
    second.sent_at = 222;
    second.samples.push_back(makeObservation("station_two", 202, 33.0f, 44.0f, 1002.0f));

    const char* first_json = ObservationSerializer::toJson(first);
    REQUIRE(first_json != nullptr);

    std::string first_copy = first_json;

    const char* second_json = ObservationSerializer::toJson(second);
    REQUIRE(second_json != nullptr);

    CHECK(first_copy != std::string(second_json));
    CHECK(std::string(second_json).find("station_two") != std::string::npos);
}
