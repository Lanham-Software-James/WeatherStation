#include "doctest.h"

#include "../test/mocks/MockLogger.h"

TEST_CASE("MockLogger stores println messages")
{
    MockLogger logger;

    logger.println("hello");

    REQUIRE(logger.entries.size() == 1);
    CHECK(logger.entries[0] == "hello");
}

TEST_CASE("MockLogger stores print values")
{
    MockLogger logger;

    logger.print("count=");
    logger.print(42);
    logger.print(100UL);
    logger.print(3.5f);

    REQUIRE(logger.entries.size() == 4);
    CHECK(logger.entries[0] == "count=");
    CHECK(logger.entries[1] == "42");
    CHECK(logger.entries[2] == "100");
}

TEST_CASE("MockLogger contains finds stored text")
{
    MockLogger logger;

    logger.println("network disconnected");

    CHECK(logger.contains("network"));
    CHECK(logger.contains("disconnected"));
    CHECK_FALSE(logger.contains("publisher"));
}
