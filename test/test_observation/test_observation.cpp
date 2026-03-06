#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "model/Observation.h"

TEST_CASE("Observation initializes with default values") {
    Observation obs;

    CHECK(obs.temperature == 0.0f);
    CHECK(obs.humidity == 0.0f);
    CHECK(obs.pressure == 0.0f);
}

TEST_CASE("Observation can be assigned values implicitly") {
    Observation obs(1013.25f, 25.5f, 60.0f);

    CHECK(obs.pressure == 1013.25f);
    CHECK(obs.temperature == 25.5f);
    CHECK(obs.humidity == 60.0f);    
}

TEST_CASE("Observation can be assigned values explicitly") {
    Observation obs;
    obs.pressure = 1013.25f;
    obs.temperature = 25.5f;
    obs.humidity = 60.0f;

    CHECK(obs.pressure == 1013.25f);
    CHECK(obs.temperature == 25.5f);
    CHECK(obs.humidity == 60.0f);    
}
