#include "doctest.h"

#include <cstring>
#include <string>

#include "model/Observation.h"
#include "publisher/HttpPublisher.h"
#include "../test/mocks/MockHttpClient.h"
#include "../test/mocks/MockNetworkStatus.h"
#include "../test/mocks/MockLogger.h"

TEST_CASE("HttpPublisher returns correct name")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    CHECK(std::string(publisher.getName()) == "HttpPublisher");
}

TEST_CASE("HttpPublisher initialize fails when endpoint is null")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher(nullptr, &network_status, &http_client, &logger);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
}

TEST_CASE("HttpPublisher initialize fails when network status is null")
{
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", nullptr, &http_client, &logger);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
}

TEST_CASE("HttpPublisher initialize fails when HTTP client is null")
{
    MockNetworkStatus network_status;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, nullptr, &logger);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
}

TEST_CASE("HttpPublisher initialize fails when logger is null")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, nullptr);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
}

TEST_CASE("HttpPublisher initialize fails when network is disconnected")
{
    MockNetworkStatus network_status;
    network_status.connected = false;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
}

TEST_CASE("HttpPublisher initialize succeeds with valid dependencies and connected network")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    CHECK(publisher.initialize() == true);
    CHECK(publisher.isInitialized() == true);
}

TEST_CASE("HttpPublisher publish fails before initialization")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    Observation observation{};
    observation.station_id = "home_ref";

    CHECK(publisher.publish(observation) == false);
}

TEST_CASE("HttpPublisher publish fails when network is disconnected")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    network_status.connected = false;

    Observation observation{};
    observation.station_id = "home_ref";

    CHECK(publisher.publish(observation) == false);
    CHECK(http_client.begin_called == false);
    CHECK(http_client.post_called == false);
    CHECK(http_client.end_called == false);
}

TEST_CASE("HttpPublisher publish fails when HTTP begin fails")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.begin_result = false;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";
    observation.sequence_number = 1;
    observation.timestamp_utc = 1000;
    observation.temperature_c = 20.0f;
    observation.humidity_pct = 50.0f;
    observation.pressure_hpa = 1013.25f;

    CHECK(publisher.publish(observation) == false);
    CHECK(http_client.begin_called == true);
    CHECK(http_client.post_called == false);
    CHECK(http_client.end_called == false);
}

TEST_CASE("HttpPublisher publish adds JSON content type header")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";
    observation.sequence_number = 1;
    observation.timestamp_utc = 1000;
    observation.temperature_c = 20.0f;
    observation.humidity_pct = 50.0f;
    observation.pressure_hpa = 1013.25f;

    REQUIRE(publisher.publish(observation) == true);

    CHECK(http_client.add_header_called == true);
    REQUIRE(http_client.last_header_name != nullptr);
    REQUIRE(http_client.last_header_value != nullptr);
    CHECK(std::string(http_client.last_header_name) == "Content-Type");
    CHECK(std::string(http_client.last_header_value) == "application/json");
}

TEST_CASE("HttpPublisher publish sends serialized observation payload")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";
    observation.sequence_number = 42;
    observation.timestamp_utc = 1700000000;
    observation.temperature_c = 21.52f;
    observation.humidity_pct = 48.10f;
    observation.pressure_hpa = 1012.32f;

    REQUIRE(publisher.publish(observation) == true);

    CHECK(http_client.post_called == true);
    CHECK(std::strstr(http_client.last_payload, "\"station_id\":\"home_ref\"") != nullptr);
    CHECK(std::strstr(http_client.last_payload, "\"sequence_number\":42") != nullptr);
    CHECK(std::strstr(http_client.last_payload, "\"timestamp_utc\":1700000000") != nullptr);
    CHECK(std::strstr(http_client.last_payload, "\"temperature_c\":21.52") != nullptr);
    CHECK(std::strstr(http_client.last_payload, "\"humidity_pct\":48.10") != nullptr);
    CHECK(std::strstr(http_client.last_payload, "\"pressure_hpa\":1012.32") != nullptr);
}

TEST_CASE("HttpPublisher publish fails when POST returns non-positive response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = -1;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";
    observation.sequence_number = 2;
    observation.timestamp_utc = 2000;
    observation.temperature_c = 22.0f;
    observation.humidity_pct = 55.0f;
    observation.pressure_hpa = 1011.0f;

    CHECK(publisher.publish(observation) == false);
    CHECK(http_client.begin_called == true);
    CHECK(http_client.post_called == true);
    CHECK(http_client.end_called == true);
}

TEST_CASE("HttpPublisher publish succeeds for 200 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 200;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";

    CHECK(publisher.publish(observation) == true);
    CHECK(http_client.end_called == true);
}

TEST_CASE("HttpPublisher publish succeeds for 201 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 201;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";

    CHECK(publisher.publish(observation) == true);
}

TEST_CASE("HttpPublisher publish succeeds for 204 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 204;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";

    CHECK(publisher.publish(observation) == true);
}

TEST_CASE("HttpPublisher publish fails for 400 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 400;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";

    CHECK(publisher.publish(observation) == false);
    CHECK(http_client.end_called == true);
}

TEST_CASE("HttpPublisher publish fails for 500 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 500;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    Observation observation{};
    observation.station_id = "home_ref";

    CHECK(publisher.publish(observation) == false);
    CHECK(http_client.end_called == true);
}
