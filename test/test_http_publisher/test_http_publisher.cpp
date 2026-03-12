#include "doctest.h"

#include <string>

#include "model/Observation.h"
#include "model/ObservationBatch.h"
#include "publisher/HttpPublisher.h"
#include "mocks/MockHttpClient.h"
#include "mocks/MockNetworkStatus.h"
#include "mocks/MockLogger.h"

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

    ObservationBatch makeBatch()
    {
        ObservationBatch batch{};
        batch.station_id = "home_ref";
        batch.sent_at = 1700000060;
        batch.samples.push_back(makeObservation("home_ref", 1700000000, 21.52f, 48.10f, 1012.32f));
        batch.samples.push_back(makeObservation("home_ref", 1700000010, 21.60f, 48.00f, 1012.28f));
        return batch;
    }
}

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
    CHECK(logger.contains("endpoint is null"));
}

TEST_CASE("HttpPublisher initialize fails when network status is null")
{
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", nullptr, &http_client, &logger);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(logger.contains("network status is null"));
}

TEST_CASE("HttpPublisher initialize fails when HTTP client is null")
{
    MockNetworkStatus network_status;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, nullptr, &logger);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(logger.contains("HTTP client is null"));
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
    CHECK(logger.contains("network not connected"));
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

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == false);
}

TEST_CASE("HttpPublisher publish fails when network is disconnected")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);
    network_status.connected = false;

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == false);
    CHECK(http_client.begin_called == false);
    CHECK(http_client.post_called == false);
    CHECK(http_client.end_called == false);
    CHECK(logger.contains("network disconnected"));
}

TEST_CASE("HttpPublisher publish fails when HTTP begin fails")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.begin_result = false;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == false);
    CHECK(http_client.begin_called == true);
    CHECK(http_client.post_called == false);
    CHECK(http_client.end_called == false);
    CHECK(logger.contains("failed to begin HTTP connection"));
}

TEST_CASE("HttpPublisher publish adds JSON content type header")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    REQUIRE(publisher.publish(batch) == true);

    CHECK(http_client.add_header_called == true);
    REQUIRE(http_client.last_header_name != nullptr);
    REQUIRE(http_client.last_header_value != nullptr);
    CHECK(std::string(http_client.last_header_name) == "Content-Type");
    CHECK(std::string(http_client.last_header_value) == "application/json");
}

TEST_CASE("HttpPublisher publish calls begin with configured endpoint")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    const char* endpoint = "https://example.com";
    HttpPublisher publisher(endpoint, &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    REQUIRE(publisher.publish(batch) == true);

    REQUIRE(http_client.last_url != nullptr);
    CHECK(std::string(http_client.last_url) == endpoint);
}

TEST_CASE("HttpPublisher publish sends serialized batch payload")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    REQUIRE(publisher.publish(batch) == true);

    CHECK(http_client.post_called == true);
    CHECK(http_client.last_payload.find("\"station_id\":\"home_ref\"") != std::string::npos);
    CHECK(http_client.last_payload.find("\"sent_at\":\"") != std::string::npos);
    CHECK(http_client.last_payload.find("\"samples\":[") != std::string::npos);
    CHECK(http_client.last_payload.find("\"ts\":\"") != std::string::npos);
    CHECK(http_client.last_payload.find("\"temperature_c\":21.52") != std::string::npos);
    CHECK(http_client.last_payload.find("\"humidity_pct\":48.10") != std::string::npos);
    CHECK(http_client.last_payload.find("\"pressure_hpa\":1012.32") != std::string::npos);
}

TEST_CASE("HttpPublisher publish fails when POST returns non-positive response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = -1;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == false);
    CHECK(http_client.begin_called == true);
    CHECK(http_client.post_called == true);
    CHECK(http_client.end_called == true);
    CHECK(logger.contains("HTTP POST failed"));
}

TEST_CASE("HttpPublisher publish succeeds for 200 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 200;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == true);
    CHECK(http_client.end_called == true);
    CHECK(logger.contains("HTTP response"));
}

TEST_CASE("HttpPublisher publish succeeds for 201 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 201;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == true);
}

TEST_CASE("HttpPublisher publish succeeds for 204 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 204;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == true);
}

TEST_CASE("HttpPublisher publish fails for 400 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 400;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == false);
    CHECK(http_client.end_called == true);
    CHECK(logger.contains("HTTP response"));
}

TEST_CASE("HttpPublisher publish fails for 500 response")
{
    MockNetworkStatus network_status;
    MockHttpClient http_client;
    MockLogger logger;
    http_client.post_result = 500;

    HttpPublisher publisher("https://example.com", &network_status, &http_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == false);
    CHECK(http_client.end_called == true);
    CHECK(logger.contains("HTTP response"));
}
