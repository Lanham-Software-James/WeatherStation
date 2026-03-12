#include "doctest.h"

#include "config/StationConfig.h"
#include "sensors/SensorFactory.h"
#include "../test/mocks/MockConstructedSensor.h"

TEST_CASE("SensorFactory creates enabled sensors only")
{
    SensorFactory factory(
        [](const SensorConfig& config) { return new MockConstructedSensor("SHT41Sensor", config.id); },
        [](const SensorConfig& config) { return new MockConstructedSensor("BMP280Sensor", config.id); }
    );

    StationConfig config{};
    config.sensors = {
        {SensorType::SHT41, "sht41_main", true, 21, 22, 0x44, 0.0f, 0.0f, 0.0f},
        {SensorType::BMP280, "bmp280_main", false, 21, 22, 0x76, 0.0f, 0.0f, 0.0f}
    };

    std::vector<Sensor*> sensors = factory.createSensors(config);

    REQUIRE(sensors.size() == 1);
    CHECK(std::string(sensors[0]->getName()) == "SHT41Sensor");
    CHECK(std::string(sensors[0]->getId()) == "sht41_main");

    for (Sensor* sensor : sensors)
    {
        delete sensor;
    }
}

TEST_CASE("SensorFactory preserves sensor order")
{
    SensorFactory factory(
        [](const SensorConfig& config) { return new MockConstructedSensor("SHT41Sensor", config.id); },
        [](const SensorConfig& config) { return new MockConstructedSensor("BMP280Sensor", config.id); }
    );

    StationConfig config{};
    config.sensors = {
        {SensorType::SHT41, "sht41_main", true},
        {SensorType::BMP280, "bmp280_main", true}
    };

    auto sensors = factory.createSensors(config);

    REQUIRE(sensors.size() == 2);

    CHECK(std::string(sensors[0]->getName()) == "SHT41Sensor");
    CHECK(std::string(sensors[1]->getName()) == "BMP280Sensor");

    for (auto* s : sensors) delete s;
}

TEST_CASE("SensorFactory preserves sensor order")
{
    SensorFactory factory(
        [](const SensorConfig& config) { return new MockConstructedSensor("SHT41Sensor", config.id); },
        [](const SensorConfig& config) { return new MockConstructedSensor("BMP280Sensor", config.id); }
    );

    StationConfig config{};
    config.sensors = {
        {SensorType::SHT41, "sht41_main", true},
        {SensorType::BMP280, "bmp280_main", true}
    };

    auto sensors = factory.createSensors(config);

    REQUIRE(sensors.size() == 2);

    CHECK(std::string(sensors[0]->getName()) == "SHT41Sensor");
    CHECK(std::string(sensors[1]->getName()) == "BMP280Sensor");

    for (auto* s : sensors) delete s;
}

TEST_CASE("SensorFactory skips disabled sensors")
{
    SensorFactory factory(
        [](const SensorConfig& config) { return new MockConstructedSensor("SHT41Sensor", config.id); },
        [](const SensorConfig& config) { return new MockConstructedSensor("BMP280Sensor", config.id); }
    );

    StationConfig config{};
    config.sensors = {
        {SensorType::SHT41, "a", false},
        {SensorType::BMP280, "b", true},
        {SensorType::SHT41, "c", false}
    };

    auto sensors = factory.createSensors(config);

    REQUIRE(sensors.size() == 1);
    CHECK(std::string(sensors[0]->getId()) == "b");

    for (auto* s : sensors) delete s;
}

TEST_CASE("SensorFactory handles missing creator")
{
    SensorFactory factory(
        nullptr,
        [](const SensorConfig& config) { return new MockConstructedSensor("BMP280Sensor", config.id); }
    );

    StationConfig config{};
    config.sensors = {
        {SensorType::SHT41, "sht41_main", true},
        {SensorType::BMP280, "bmp280_main", true}
    };

    auto sensors = factory.createSensors(config);

    REQUIRE(sensors.size() == 1);
    CHECK(std::string(sensors[0]->getName()) == "BMP280Sensor");

    for (auto* s : sensors) delete s;
}

TEST_CASE("SensorFactory passes config id to sensor constructor")
{
    SensorFactory factory(
        [](const SensorConfig& config) { return new MockConstructedSensor("SHT41Sensor", config.id); },
        [](const SensorConfig& config) { return new MockConstructedSensor("BMP280Sensor", config.id); }
    );

    StationConfig config{};
    config.sensors = {
        {SensorType::SHT41, "alpha", true},
        {SensorType::BMP280, "beta", true}
    };

    auto sensors = factory.createSensors(config);

    REQUIRE(sensors.size() == 2);

    CHECK(std::string(sensors[0]->getId()) == "alpha");
    CHECK(std::string(sensors[1]->getId()) == "beta");

    for (auto* s : sensors) delete s;
}

