#include "doctest.h"

#include "../test/mocks/MockClock.h"

TEST_CASE("MockClock initializes with default values")
{
    MockClock clock;

    CHECK(clock.millis() == 0UL);
    CHECK(clock.now() == 0);
}

TEST_CASE("MockClock returns explicitly set millis value")
{
    MockClock clock;

    clock.setMillis(12345UL);

    CHECK(clock.millis() == 12345UL);
}

TEST_CASE("MockClock returns explicitly set time value")
{
    MockClock clock;

    clock.setNow(1700000000);

    CHECK(clock.now() == 1700000000);
}

TEST_CASE("MockClock advanceMillis increments current millis")
{
    MockClock clock;

    clock.advanceMillis(100UL);
    CHECK(clock.millis() == 100UL);

    clock.advanceMillis(250UL);
    CHECK(clock.millis() == 350UL);
}

TEST_CASE("MockClock setMillis overwrites previous millis value")
{
    MockClock clock;

    clock.advanceMillis(500UL);
    CHECK(clock.millis() == 500UL);

    clock.setMillis(42UL);
    CHECK(clock.millis() == 42UL);
}

TEST_CASE("MockClock setNow overwrites previous time value")
{
    MockClock clock;

    clock.setNow(1000);
    CHECK(clock.now() == 1000);

    clock.setNow(2000);
    CHECK(clock.now() == 2000);
}

TEST_CASE("MockClock millis and now are independent")
{
    MockClock clock;

    clock.setMillis(1234UL);
    clock.setNow(1700000000);

    CHECK(clock.millis() == 1234UL);
    CHECK(clock.now() == 1700000000);

    clock.advanceMillis(66UL);

    CHECK(clock.millis() == 1300UL);
    CHECK(clock.now() == 1700000000);
}

TEST_CASE("MockClock can be used through Clock interface")
{
    MockClock mock_clock;
    Clock* clock = &mock_clock;

    mock_clock.setMillis(777UL);
    mock_clock.setNow(1700000123);

    CHECK(clock->millis() == 777UL);
    CHECK(clock->now() == 1700000123);
}
