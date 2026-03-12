#include "doctest.h"

#include <string>

#include "mocks/MockHttpClient.h"

TEST_CASE("MockHttpClient records begin call")
{
    MockHttpClient http_client;

    CHECK(http_client.begin("https://example.com") == true);
    CHECK(http_client.begin_called == true);
    REQUIRE(http_client.last_url != nullptr);
    CHECK(std::string(http_client.last_url) == "https://example.com");
}

TEST_CASE("MockHttpClient records header values")
{
    MockHttpClient http_client;

    http_client.addHeader("Content-Type", "application/json");

    CHECK(http_client.add_header_called == true);
    REQUIRE(http_client.last_header_name != nullptr);
    REQUIRE(http_client.last_header_value != nullptr);
    CHECK(std::string(http_client.last_header_name) == "Content-Type");
    CHECK(std::string(http_client.last_header_value) == "application/json");
}

TEST_CASE("MockHttpClient records POST payload")
{
    MockHttpClient http_client;

    CHECK(http_client.post("{\"ok\":true}") == 200);
    CHECK(http_client.post_called == true);
    CHECK(http_client.last_payload == "{\"ok\":true}");
}

TEST_CASE("MockHttpClient records end call")
{
    MockHttpClient http_client;

    http_client.end();

    CHECK(http_client.end_called == true);
}
